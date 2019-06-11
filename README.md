# README:

### Maze Challenge

**Contributors:** Adi Srivastava, Ben Eisner, Abenezer Dara, Junfei Yu

The maze challenge program is a client that solves mazes produced by the Dartmouth CS server. `Avatars` is responsible for initiating contact with the server and spinning up a separate process for each avatar. It is also responsible for the logic involved with a single avatar moving through the maze. See `DESIGN.md` for more.

## Compiling and running

To compile, run `make`.

### Usage

`./Avatars AvatarId nAvatars Difficulty Hostname MazePort logFile`

| argument            | explanation              |
| ------------------- | ------------------------ |
| `AvatarId (int)`    | ID of first avatar       |
| `nAvatars (int)`    | number of avatars        |
| `Difficulty (int)`  | difficulty of maze (0-9) |
| `Hostname (char *)` | IP address of server     |
| `logFile (char *)`  | filename for log         |

**example command lines:**

* `./Avatars 0 3 2 129.170.212.235 17235 Amazing_group16_3_2.log`
* `./Avatars 0 3 2 flume 10829 Amazing_group16_3_2.log`
* `./Avatars 0 9 9 flume.cs.dartmouth.edu 17235 Amazing_group16_9_9.log`

### Exit status

| status | meaning       |
| ------ | ------------- |
| 0      | success       |
| 1      | bad arguments |
| 2      | socket error  |
| 3      | forking error |
| 4      | send error    |
| 5      | read error    |

## Limitations

* The wall follower algorithm (see `DESIGN.md` for more) does not guarantee the shortest path. Therefore, there exist (unlikely) scenarios in which our client runs out of moves.
