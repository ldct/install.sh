# install.sh
monitored install scripts

## hack

1) server sets up and launches lsh process
2) lsh sets stdin and stdout to same fd
3) server monitors that fd and sends updates to client when it changes

## how to do things properly

1) lsh takes script name as argument
2) lsh spawns fake_cin and fake_cout program
3) master program replaces stdin and stdout with fake program fids
4) execute each command

normally, if stdout and stderr are the same fd, then

cout << "a";
cerr << "b";

must print "ab". If they are redirected to two programs, is this still true?
