# CS50 Maze Challenge
## Group 16, March 2018


### Testing Plans

#### General Test Plan:

Testing was done through an maze.sh bash script.

The aspects of Maze Client that were tested are:

	1. Checking if the command line input checks worked correctly
		1. checking usage error
		2. checking correct number of avatars
		3. checking correct difficulty was given	
	2. Then running the AMStartup with the specified maze, avatars, and logfile
		1. Run with various difficulties and avatars


#### Memory Leaks

We used myvalgrind to find and eliminate memory errors and leaks. No memory leaks or errors.

#### Limitations

The run time of this algorithm is linear-based, so with more avatars and higher difficulty, solve time increases.

