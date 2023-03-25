#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// you can make this longer or shorter (it's in seconds)
#define maxRounds 10
#define maxDamage 200
#define sleepTime 3
#define damageRange 50

// copied as a separate flag
// to each process
volatile sig_atomic_t
    damage; // changed to damage so it makes more sense for myself volatile
            // shows that this may change in the future, typically used when we
            // have multupiple threads, honestly still grasping the idea of
            // atomic but it seems like it clears up issues down the road to
            // prevent race conditions

// this is set in each process
void signal_handler(int signum) { damage = 1 + rand() % damageRange; }

void waitOnChild() // perfect no need to change
{
  int status;
  // wait for child to end.
  pid_t wpid = waitpid(-1, &status, 0);
}

void processChild() {
  // process signals
  // send parent a SIGINT when
  // the child gets one.

  int totalDamage = 0;
  int rounds = 0; // added this for better control over the while loop

  while (rounds < maxRounds && totalDamage < maxDamage) {
    if (damage) // if damage is more than 0
    {

      totalDamage += damage; // total + damage

      printf("Child %d got hit!, damage: %d, total damage: %d\n", getpid(),
             damage, totalDamage); // works now

      if (totalDamage > 200) {
        printf("THE CHILD DIED");
        exit(0);
      }
      kill(getppid(), SIGINT);
      damage = 0; // resets the damage
      // the kill process needs to be used here to send the signal to the parent
      // and informaiton that needs to be sent back over ugh
      kill(getppid(), SIGINT);
    }
    sleep(1 + rand() % sleepTime);
    rounds++;
  }
}

void processParent(pid_t child) {
  int totalDamage = 0;
  int rounds = 0;
  // Send signal to Child first
  kill(child, SIGINT);
  while (rounds < maxRounds && totalDamage < maxDamage) {
    if (damage) {
      totalDamage += damage;
      printf("parent %d got hit!!, damage: %d, total damage: %d\n", getpid(),
             damage, totalDamage); // works
      damage = 0;                  // reset
      if (totalDamage > 200) {
        printf("THE PARENT HAS DIED");
        exit(0);
      }
      kill(child, SIGINT);
    }

    sleep(1 + rand() % sleepTime);
    rounds++;
  }

  sleep(1);
  rounds++;
}

int main() {

  srand(time(NULL));
  damage = 0;

  // install signal handler for SIGINT
  signal(SIGINT, signal_handler);
  // signal handler works for both
  // child and parent.
  //  the flag (now damage) is copied between
  //  the two processes and is
  //  different in each process.

  pid_t cpid = fork(); // no need to change this is already perfect
  if (cpid == -1) {
    printf("Fork error\n");
    _exit(-1);
  }

  if (cpid > 0) // parent
  {
    sleep(1);
    processParent(cpid);
    waitOnChild();
  }

  if (cpid == 0) // child
  {
    sleep(1);
    processChild();
  }

  return 0;
}