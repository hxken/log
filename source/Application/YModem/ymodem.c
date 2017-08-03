
typedef enum {

}Event;

typedef struct {
  Event event;
  int (*exec)(Event e);
  struct _status *next_status;
}Table;

typedef struct _status {
  Table *tables;
}Status;

typedef struct {
  Status *current_status;
}FSM;


int fsm_parase(FSM *fsm, Event e)
{
  int i = 0;
  Table *tables = fsm->current_status->tables;
//  int len = sizeof(fsm->current_status->tables) / sizeof(fsm->current_status->tables[0]);

  for (; tables != NULL; tables++)
  {
    if (e == tables->event)
    {
      break;
    }
  }

  if (tables == NULL)
  {
    return -1; //无匹配
  }

  tables->exec(e);//执行状态切换函数
  fsm->current_status = tables->next_status;


}
