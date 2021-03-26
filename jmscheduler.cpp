#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <list>
using namespace std;

struct Node {
  string process = "";
  int burst = 0;
  int arrival = 0;
  int turnaroundTime = 0;
  int waitingTime = 0;
  int endTime = 0;
  bool submission = true; // if arriving. changes to false after 1st occurence

  Node(string p, int b, int a) {
    process = p;
    burst = b;
    arrival = a;
  }
};

// round robin
// non-preemptive shortest job first
// shortest remaining time first 

void averages(list<Node> l) {
  float avgtt = 0.0f;
  float avgwt = 0.0f;
  std::list<Node>::iterator it=l.begin();

  cout << endl << "Process ID" << " " << "Turnaround Time" << " " << "Waiting Time" << endl;
  for(it=l.begin(); it!=l.end();it++) {
    avgtt += it->turnaroundTime;
    avgwt += it->waitingTime;

    cout << it->process << "          " << 
    it->turnaroundTime << "           " << 
    it->waitingTime << endl;
  }
  cout << "Average" << 
  "    " << avgtt << "/" << l.size() << " = " << avgtt/l.size() <<
  "    " << avgwt << "/" << l.size() << " = " << avgwt/l.size() << endl << endl;
}

void roundRobin(list<Node> l) {
  int quantum = 3;
  int incomplete = 1;
  int time = 0;

  cout << "Round Robin Scheduling" << endl;

  std::list<Node>::iterator it=l.begin();

  while(incomplete == 1) {
    for(it=l.begin(); it!=l.end(); it++) {
      if (it->burst > 0) {
        cout << time << " " << it->process << " ";

        if(it->waitingTime == 0) it->waitingTime = time - it->arrival;
        else it->waitingTime += time - it->endTime;

        // if burst<=3, increase time by burst (1,2, or 3), hardcode burst = 0 instead of negative
        if (it->burst <= 3) {time += it->burst; it->burst = 0; it->endTime = time;}
        // else automatically increase by quantum
        else {time += quantum; it->burst -= quantum; it->endTime = time; }

        // if any node burst > 0, incomplete++
        incomplete++;

        // if burst = 0, process terminated
        if(it->burst <= 0) {
          cout << "Process terminated" << endl;
          it->turnaroundTime = it->endTime - it->arrival;
        }
        else  cout << "Quantum expired" << endl;
      }
    }

    // incomplete resets to 1 if any burst > 0, start from beginning of list
    if(incomplete > 1) incomplete = 1;
    else incomplete = 0;
  }
  cout << time << " Complete" << endl;
  averages(l);
}

void SJF(list<Node> l) {
  int incomplete = 1;
  int time = 0;

  cout << "SJF Scheduling" << endl;

  std::list<Node>::iterator it=l.begin();
  string tempID = "";
  int tempBurst = 9999;


// if any node has burst > 0, incomplete++
  while(incomplete == 1) {
    for (it=l.begin(); it!=l.end(); ++it) {
      if(it->burst > 0) incomplete++; 

      // identify shortest burst time
      if (it->burst < tempBurst && it->burst > 0 && it->arrival <= time) {
        tempID = it->process;
        tempBurst = it->burst;
      }
    }
    
    // get waiting time, complete job (burst = 0), add burst to time, get turnaround time
    for(it=l.begin(); it!=l.end(); ++it) {
      if (tempID == it->process) {
        cout << time << " " << it->process << "     Process Terminated" << endl;
        it->waitingTime = time -it->arrival;
        time+=it->burst;
        it->turnaroundTime = time - it->arrival;
        it->burst = 0;
      }
    }

    // if any burst>0, reset
    if(incomplete > 1) {
      incomplete = 1;
      tempID = "";
      tempBurst = 9999;
    }
    else if(incomplete == 1) incomplete = 0;
  }
  cout << time << " Complete" << endl;
  averages(l);
}
 
void SRTF(list<Node>  l) {
  int incomplete = 1;
  int time = 0;

  cout << "SRTF Scheduling" << endl;

  
  std::list<Node>::iterator it = l.begin();
  string tempID = "";
  int tempBurst = 9999;
  int tempTime = 0;

  string temp2ID = "";
  int temp2Burst = 9999;
  int tempArrival = 0;

  while (incomplete == 1){
    for (it=l.begin(); it!=l.end(); ++it) {
      if(it->burst > 0) incomplete++; 

      // like sjf, identify shortest job + current time is > than arrival time
      if (it->burst < tempBurst && it->burst > 0 && it->arrival <= time) {
        tempID = it->process;
        tempBurst = it->burst;
      }
    }
    
    // identify next shortest burst time 
    for (it=l.begin(); it!=l.end(); ++it) {
      // if not the first PID && arrival time < time + 1st job burst 
      if (it->process != tempID && it->arrival <= time + tempBurst && 
      it->burst > 0 && it->burst < temp2Burst) {
        temp2ID = it->process;
        temp2Burst = it->burst;
        tempArrival = it->arrival;

        // if time+1st burst > 2nd job arrival, it can be scheduled
        // if scheduled, is 2nd job burst < time+1stburst - arrival time of 2nd - time
        // if so, burst up until 2nd job
        if(time+tempBurst > tempArrival && temp2Burst < tempBurst-(tempArrival-time)) {
          tempBurst = tempArrival - time;
        }
      }
    }

    for (it=l.begin(); it!=l.end(); ++it) {
      if (tempID == it->process) {
        cout << time << "     " << it->process << "     ";
        if(it->submission == true) {it->submission=false; it->waitingTime = time - it->arrival;}
        else it->waitingTime += time - it->endTime;
        it->burst -= tempBurst;
        if(it->burst > 0) cout << "Process preempted by process with shorter burst time" << endl;
        else cout << "Process Terminated" << endl;
        time+=tempBurst;
        it->endTime = time;
        it->turnaroundTime = it->endTime - it->arrival;
      }
    }

    if(incomplete > 1) {
      incomplete = 1;
      tempID="";
      tempBurst=9999;
      int tempTime=0;
      string temp2ID="";
      int temp2Burst=9999;
      int tempArrival=0;
    }
    else incomplete = 0;
  }
  cout << time << " Complete" << endl;
  averages(l);
}

int main (int argc, char *argv[]) {

  if ( argc != 2 ) // argc should be 2 for correct execution
    // We print argv[0] assuming it is the program name
    cout<<"usage: "<< argv[0] <<" <filename>\n";

  else {
    // We assume argv[1] is a filename to open
    ifstream myfile ( argv[1] );

    // Always check to see if file opening succeeded
    if ( !myfile.is_open() )cout<<"Could not open file\n";

    else {
      string line;
      string delimiter = " ";

      string pid;
      int burstTime;
      int arrivalTime;
      string s;
      const char *temp;

      list<Node> program;

      while(getline(myfile,line)){
        int start = 0;
        int end = line.find(delimiter);

        for(int i = 0; end != line.npos; i++) {
          if(i == 0) pid = line.substr(start, end - start);
          if(i == 1) {
            s = line.substr(start, end - start);
            temp = s.c_str();
            burstTime = atoi(temp);
            }

          start = end + delimiter.length();
          end = line.find(delimiter, start);
          
          if (end == -1) {
            s = line.substr(start, end - start);
            temp = s.c_str();
            arrivalTime = atoi(temp);
            }
        } 

        Node temp(pid,burstTime,arrivalTime);
        program.push_back(temp);
      }

      myfile.close();

      roundRobin(program);
      SJF(program);
      SRTF(program);
    }
  }
}