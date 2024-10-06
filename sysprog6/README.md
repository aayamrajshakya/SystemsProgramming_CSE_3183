# Program 6: Parent-Child Process with Signal Handling

This program is a reimagining of Program 4, utilizing the "angl.dat" file from Program 3. It creates a parent-child process structure where both processes handle different tasks and communicate via signals.

## Parent Process Behavior

### Signals to Handle:

1. **SIGCHLD**: 
   - Use `wait` or `waitpid` to clean up terminated children.
   - If no children remain, print a message and terminate.

2. **SIGINT**:
   - Print "Exit: Are you sure (Y/n)?"
   - Exit if user enters 'Y', otherwise continue.

3. **SIGUSR1**:
   - Print "Warning! roll outside of bounds"
   - Return to pause state.

4. **SIGUSR2**:
   - Print "Warning! pitch outside of bounds"
   - Return to pause state.

### Main Function Behavior:
- Set up all signal handlers
- Spawn the child process
- Enter a pause loop
- Print status messages throughout

### Note:
Consider setting up an exit handler to send SIGTERM to any running children, as SIGINT from ctrl-c is sent to both parent and children by default.

## Child Process Behavior

### Signals to Handle:

1. **SIGINT**: 
   - Block this signal in the child's signal mask.

2. **SIGTERM**: 
   - Terminate the child when the parent dies.

3. **None or SIGALRM** (implementation dependent)

### Main Function Behavior:
- Similar to Programs 4 and 5
- Open "angl.dat"
- Read 1 triple every second
- Determine if pitch and roll are outside bounds
- Instead of printing error messages, send signals to the parent:
  - Send SIGUSR1 if roll is outside -20.0 to 20.0 range
  - Send SIGUSR2 if pitch is outside -20.0 to 20.0 range
