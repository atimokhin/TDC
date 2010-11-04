#ifndef  WATCHER_FILE_H
#define  WATCHER_FILE_H

#include <string>
#include "watcher.h"

class WatcherFile: public Watcher
{
public:
  WatcherFile(std::string dir): _OutputDir(dir+"/") {};

  int Watch();

private:
  std::string _OutputDir;
};

#endif
