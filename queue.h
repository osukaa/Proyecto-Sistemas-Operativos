/*
 *  FILE   : queue.h
 *  AUTHOR : Jeffrey Hunter
 *  WEB    : http://www.iDevelopment.info
 *  NOTES  : Define queue record structure and
 *           all forward declarations.
 */

#include <stdio.h>
#include <stdlib.h>

#define Error(Str)        FatalError(Str)
#define FatalError(Str)   fprintf(stderr, "%s\n", Str), exit(1)

typedef int ElementType;

  struct QueueRecord {
  int Capacity;
  int Front;
  int Rear;
  int Size;
  ElementType *Array;
  };
  typedef struct QueueRecord *Queue;

  int         IsEmpty(Queue Q);
  int         IsFull(Queue Q);
  Queue       CreateQueue(int MaxElements);
  void        DisposeQueue(Queue Q);
  void        MakeEmpty(Queue Q);
  void        Enqueue(ElementType X, Queue Q);
  ElementType Front(Queue Q);
  void        Dequeue(Queue Q);
  ElementType FrontAndDequeue(Queue Q);