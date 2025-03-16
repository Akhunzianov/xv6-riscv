#ifndef PROCINFO_H
#define PROCINFO_H

struct procinfo {
  int pid;                         
  char name[16];       
  int state;                       
  int ppid;                        
  char pname[16];
};

#endif