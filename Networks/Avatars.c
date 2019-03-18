/* Avatar.c -- everything for the avatars clients will be running in this file.
 *
 * GROUP 16: Junfei Yu, Aditya Srivastava, Benjamin Eisner, Abenezer D. Dara
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>       // read, write, close
#include <string.h>       // strlen
#include <strings.h>        // bcopy, bzero
#include <netdb.h>        // socket-related structures
#include <arpa/inet.h>        // socket-related calls
#include <sys/select.h>  
#include <stdio.h>
#include <time.h>  // for random seed
#include "../Networks/amazing.h"

// check whether a move is successful of an avatar.
bool move_success(XYPos new_Pos[], XYPos old_Pos[], int nAvatars, int avatar_id) 
{
  for (int i=0; i < nAvatars; i++) {
    if((new_Pos[i].x != old_Pos[i].x) || (new_Pos[i].y != old_Pos[i].y)) {
      return true;
    } 
  }
  return false; 
}


// This returns the next move for each avatar
// based upon the classical right hand rule implementation.
// Input ex.: 1, 
// Output ex.: 1, 2, 3， 4. This number indicates which direction you should 
// go next. 
int nextMove(int previous_move, int last_move_success)
{
    int move; // if move is 0, previous move is not successful; 
              // if move is 1, previous move is successful.

    if (previous_move == M_NULL_MOVE) {
      previous_move = M_EAST;
    }

    if(last_move_success != 0) {
      if (previous_move == M_EAST) {
        move = M_SOUTH;    
      }
      else if(previous_move == M_WEST){
        move = M_NORTH;      
      }
      else if(previous_move == M_SOUTH){
        move = M_WEST;    
      } else {
        move = M_EAST;
      }
    }

    if((last_move_success == 0) && (previous_move == M_EAST)){
      previous_move = M_NORTH; 
    } else if((last_move_success == 0) && (previous_move == M_NORTH)) {
      previous_move = M_WEST;
    } else if((last_move_success == 0) && (previous_move == M_WEST)) {
      previous_move = M_SOUTH;
    } else if((last_move_success == 0) && (previous_move == M_SOUTH)) {
      previous_move = M_EAST;
    } 

    if(last_move_success != 0) {
      previous_move = move;

    }

    return previous_move;
}

// This is the helper function which will be used in the fork()
void fork_helper(int index,  struct hostent *hostp, int MazePort, int MazeHeight, int MazeWidth, int nAvatars, char * Filename) 
{
  // set up the attributes for the avatars, socket number, mazeport.
  int avatar_id = index;
  int avatar_sock = socket(AF_INET, SOCK_STREAM, 0);
 
  struct sockaddr_in themp_turn_message;
  themp_turn_message.sin_family = AF_INET;
  bcopy(hostp->h_addr_list[0], &themp_turn_message.sin_addr, hostp->h_length);
  themp_turn_message.sin_port = htons(MazePort);
  // connect to the server.
  if( (connect(avatar_sock, (struct sockaddr *) &themp_turn_message, sizeof(themp_turn_message) )) < 0) {
    printf("This is temp->sock_fd %d connection...\n", avatar_sock);
    perror("Error happened in connect\n");
    exit(1);
  } 

  AM_Message message_ready;
  memset(&message_ready, 0, sizeof(AM_Message));
  message_ready.type = htonl( (uint32_t) AM_AVATAR_READY);
  message_ready.avatar_ready.AvatarId = htonl(avatar_id);

  int temp_sendto = 0;

  if( (temp_sendto  = write(avatar_sock, &message_ready, sizeof(AM_Message))) < 0) {
    perror("sending in datagram socket");
    exit(6);
  } 

  // every avatar receives an avatar turn message from the server.
  int turn_id;
  int first_iteration = 0; // Indicates if it is the first time in the iteration.
  XYPos avartar_old_pos[nAvatars]; // This is the container for holding the old xy coordinates compared to the current coordinates.
  //int old_move = 3; // our last move
  int best_move; // our best move that we got from the herusitc function.
  int old_turnID;
  XYPos * goal = malloc (sizeof(XYPos));

  while(1)
  {
    AM_Message message_turn;
    int nbytes = read( avatar_sock, &message_turn, sizeof(AM_Message));
    if (nbytes < 0) {
      //printf("The number of nbytes is %d\n", nbytes);
      perror("receiving from socket");
      free(goal);
      exit(1);
    } else {
      if(AM_AVATAR_TURN == ntohl(message_turn.type)) {

      } else {
        if (AM_TOO_MANY_MOVES == ntohl(message_turn.type) ){
          printf("too many moves!!!! \n");
          free(goal);
          exit(20); 
        }
        if (AM_MAZE_SOLVED == ntohl(message_turn.type)){
            printf("YAY MAZE SOLVED!\n");
            FILE* fp_log = fopen(Filename, "a");
           if (fp_log != NULL){
             fprintf(fp_log, "Maze Solved!!! \n" );
             fclose(fp_log);
          }
              free(goal);
            exit(10);
          }

        printf("AM_NO_SUCH_AVATAR equals to %d \n", (uint32_t)AM_NO_SUCH_AVATAR );
        if(AM_NO_SUCH_AVATAR == ntohl(message_turn.type)) {
          printf("It says no such Avatar!!!\n");
        }
        printf("The type for each avatar thread is %u\n", ntohl(message_turn.type) );
        printf("Assumption is wrong !!!\n");
      }

      // Interpreting the message from the server.
      XYPos avartar_xy_pos[nAvatars];
      turn_id = ntohl((uint32_t)message_turn.avatar_turn.TurnId);
      for(int i = 0; i <= nAvatars-1; i++) {
        avartar_xy_pos[i].x = ntohl((uint32_t)message_turn.avatar_turn.Pos[avatar_id].x);
        avartar_xy_pos[i].y = ntohl((uint32_t)message_turn.avatar_turn.Pos[avatar_id].y);
      }

      if (turn_id == avatar_id){
          FILE * fp_log = fopen(Filename, "a"); // the file we will be writing our log into.
         if(fp_log == NULL) //if file does not exist, create it
           {
             fprintf(stderr, "Invalid log file!\n" );
             exit(200);
           }

        if (first_iteration != 0) {
          if(move_success(avartar_xy_pos, avartar_old_pos, nAvatars, old_turnID)) {
            best_move =  nextMove(best_move, 1); 
            fprintf(fp_log, "Avatar %d moved from (%d, %d) to (%d, %d) \n", turn_id, avartar_old_pos->x, avartar_old_pos->y, avartar_xy_pos[turn_id].x , avartar_xy_pos[turn_id].y);
            // get the best move for the case where last move was successful.
          } else {
            // printf("didn't move correctly\n");
            best_move =  nextMove(best_move, 0); 
            // get the best move for the case where last move was successful.
          }
        } else {
          //goal = find_goal(message_turn.avatar_turn.Pos, nAvatars);
          //goal = malloc (sizeof(XYPos));

          goal->x =  MazeWidth/2;
          goal->y =  MazeHeight/2;

          printf("Our goal is (%d, %d)\n", goal->x, goal->y);
          best_move = 3;
        }

        if(avartar_xy_pos[turn_id].x == goal->x && avartar_xy_pos[turn_id].y == goal->y) {
          best_move = M_NULL_MOVE;
          printf("Already at goal, Avatar: %d \n",  avatar_id);
        } 
        
        old_turnID = turn_id;
        
        for(int i=0; i <= nAvatars-1; i++) {
          avartar_old_pos[i] = avartar_xy_pos[i];
        }

        AM_Message message_move;
        memset(&message_move, 0, sizeof(AM_Message));
        message_move.type = htonl(AM_AVATAR_MOVE);
        message_move.avatar_move.AvatarId = htonl(avatar_id);
        // printf("BEST MOVE -> %d for avatar %d at position  (%d, %d) \n", best_move, avatar_id, 
        // ntohl((uint32_t)message_turn.avatar_turn.Pos[avatar_id].x), ntohl((uint32_t)message_turn.avatar_turn.Pos[avatar_id].y);
       
        message_move.avatar_move.Direction =  htonl(best_move); 
        int temp_sendto = 0;
        if( (temp_sendto  = write( avatar_sock, &message_move, sizeof(AM_Message))) < 0) {
          perror("sending in datagram socket");

          free(goal);
          fclose(fp_log);
          exit(6);
        }  else {
          printf("THe avatar_id is %d, the turn id is %d\n",avatar_id, turn_id);
          printf("It is not your turn!\n");
        }
        first_iteration = 1;
        fclose(fp_log);
      }
    }
  }

  free(goal);
}

// this function is for building up the log file
char * build_log_file(int nAvatars, int Difficulty) {
    // creating the log file.
    char * Filename = malloc(1024);
    strcpy(Filename, "Amazing_" );
    uid_t cur_uid = getuid();
    char str[1024];
    sprintf(str, "%d", cur_uid);
    strcat(Filename, str);
    char str_numb_avatar[1024];
    char str_difficulty[1024];
    sprintf(str_numb_avatar, "%d", nAvatars);
    sprintf(str_difficulty, "%d", Difficulty);
    strcat(Filename, "_");
    strcat(Filename, str_numb_avatar);
    strcat(Filename, "_");
    strcat(Filename, str_difficulty);
    strcat(Filename, ".log");
    return Filename;
  }

int main(int argc, char *argv[]) 
{
    //int first_move = 0;
    if(argc != 7) {
      printf("Error. Usage: AvatarId, nAvatars, Difficulty, IP address of the server, MazePort, Filename of the log.\n");
      exit(1);
    } 

    // parsing from the command line
    char *hostname; 
    int nAvatars;
    int Difficulty;
    int port = atoi(AM_SERVER_PORT);
    hostname = argv[4];
    nAvatars = atoi(argv[2]);
    Difficulty = atoi(argv[3]);
    char* Filename = argv[6];

    // values which be read from the server
    int MazePort;
    int MazeWidth;
    int MazeHeight;
    srand((unsigned)time(NULL));  

    // set up the socket address
    struct sockaddr_in themp;
    struct hostent *hostp = gethostbyname(hostname);
    if (hostp == NULL) {
      exit(3);
    }

    time_t curtime;
    time(&curtime);
    FILE* fp_log = fopen(Filename, "w+");
    if (fp_log == NULL) {
      fprintf(stderr, "%s\n", "Log file does not exist.." );
      exit(100);
    } else {
      fprintf(fp_log, "Group 16 \nTime and data: %s\n", ctime(&curtime));
      //close the file
      fclose(fp_log);

    }

    // Initialize fields of the server address
    themp.sin_family = AF_INET;
    bcopy(hostp->h_addr_list[0], &themp.sin_addr, hostp->h_length);
    themp.sin_port = htons(port);
    // Create socket
    int comm_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (comm_sock < 0) {
      perror("opening datagram socket");
      exit(2);
    }

    // connect to the server
    int success;
    if( (success = (connect(comm_sock, (struct sockaddr *) &themp, sizeof(themp) ))) < 0) {
      printf("Error happened in connect\n");
      exit(1);
    }

    // initialize the message 
    AM_Message message_init;
    memset(&message_init, 0, sizeof(AM_Message));
    message_init.type = htonl( (uint32_t) AM_INIT);
    message_init.init.nAvatars = htonl((uint32_t) nAvatars);
    message_init.init.Difficulty = htonl( (uint32_t) Difficulty);
    if( AM_INIT == ntohl((uint32_t)message_init.type)) {
      printf("Initialized \n");
    } else {
      printf("It is not an unknown message!\n");
    }
    printf("The type is %d, the number of nAvatars is %d, the Difficulty level is %d\n",  message_init.type, message_init.init.nAvatars,  message_init.init.Difficulty);
   
    // send the message 
    int temp_sendto;
    if( (temp_sendto  = write(comm_sock, &message_init, sizeof(AM_Message))) < 0) {
      perror("sending in datagram socket");
      exit(6);
    } 

    AM_Message message_init_ok;
    int nbytes = read(comm_sock, &message_init_ok, sizeof(AM_Message)+1);
    if (nbytes < 0) {
      printf("The number of nbytes is %d\n", nbytes);
      perror("receiving from socket");
      exit(1);
    } else {
      MazePort = ntohl(message_init_ok.init_ok.MazePort);
      MazeWidth = ntohl(message_init_ok.init_ok.MazeWidth);
      MazeHeight = ntohl(message_init_ok.init_ok.MazeHeight);
    }

    printf("Success init! Got these from the server: %d, %d, %d\n", MazePort, MazeWidth, MazeHeight);
    printf("------------------------------------------------------------------\n");
    for(int i=0; i <= nAvatars-1; i++) {
      if (fork()) {
        fork_helper(i, hostp, MazePort, MazeHeight, MazeWidth, nAvatars, Filename);
      }
    }

    // reap any zombies
    while (waitpid(0, NULL, WNOHANG) > 0) {
      ;
    }
    // // writing to the log file
    // FILE * fp_log = fopen(Filename, "ab+"); // the file we will be writing our log into.
    // if(fp_log == NULL) //if file does not exist, create it
    // {
    //   fp_log = fopen(Filename, "wb");
    // } 
    // 
    // time(&curtime);
    // fprintf(fp_log, "%d %d %s\n", (int)cur_uid , MazePort, ctime(&curtime));
    // fclose(fp_log);
    // printf("The filename is %s\n", Filename);
    // printf("End of the program.\n");
    return 0;
  }
