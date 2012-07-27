#ifndef HACK_H
#define HACK_H

#include "common/scummsys.h"

namespace Aesop {

uint32 buildClipping(int argc, Value *argv);
uint32 catString(int argc, Value *argv);
uint32 closeFeatureFile(int argc, Value *argv);
uint32 closeFile(int argc, Value *argv);
uint32 copyWindow(int argc, Value *argv);
uint32 createFile(int argc, Value *argv);
uint32 deleteSaves(int argc, Value *argv);
uint32 drawAutoSquare(int argc, Value *argv);
uint32 drawWalls(int argc, Value *argv);
uint32 explodeSave(int argc, Value *argv);
uint32 findLocationForMap(int argc, Value *argv);
uint32 getFeatureRecord(int argc, Value *argv);
uint32 initViewspace(int argc, Value *argv);
uint32 loadLevelMap(int argc, Value *argv);
uint32 loadVisibility(int argc, Value *argv);
uint32 lockResource(int argc, Value *argv);
uint32 long2hex(int argc, Value *argv);
uint32 openFeatureFile(int argc, Value *argv);
uint32 openFile(int argc, Value *argv);
uint32 outputTime(int argc, Value *argv);
uint32 pageFlip(int argc, Value *argv);
uint32 pause(int argc, Value *argv);
uint32 prepareSave(int argc, Value *argv);
uint32 printerOnLine(int argc, Value *argv);
uint32 randomizeArray(int argc, Value *argv);
uint32 readArrayFromFile(int argc, Value *argv);
uint32 readNumberFromFile(int argc, Value *argv);
uint32 refreshMainTextWindow(int argc, Value *argv);
uint32 rollChance(int argc, Value *argv);
uint32 saveVisibility(int argc, Value *argv);
uint32 seedRandom(int argc, Value *argv);
uint32 seekInFile(int argc, Value *argv);
uint32 sequencePlaying(int argc, Value *argv);
uint32 textBackground(int argc, Value *argv);
uint32 touch(int argc, Value *argv);
uint32 transition(int argc, Value *argv);
uint32 unlockResource(int argc, Value *argv);
uint32 updateFile(int argc, Value *argv);
uint32 walkheap(int argc, Value *argv);
uint32 windowCore(int argc, Value *argv);
uint32 writeArrayToFile(int argc, Value *argv);
uint32 writeLongToFile(int argc, Value *argv);
uint32 writeMapheaderToFile(int argc, Value *argv);
uint32 writeResourceToFile(int argc, Value *argv);
uint32 xmsallocated(int argc, Value *argv);
uint32 xmsfree(int argc, Value *argv);

} // End of namespace Aesop

#endif
