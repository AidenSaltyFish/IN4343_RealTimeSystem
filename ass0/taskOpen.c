TASK_ID taskOpen
    (
    const char *  name,         /* task name - default name will be chosen */
    int           priority,     /* task priority */
    int           options,      /* VX_ task option bits */
    int           mode,         /* object management mode bits */
    char *        pStackBase,   /* location of execution stack */
    size_t        stackSize,    /* execution stack size */
    void *        context,      /* context value */
    FUNCPTR       entryPt,      /* entry point of new task */
    _Vx_usr_arg_t arg1,         /* argument 1 to pass to entryPt */
    ...,
    _Vx_usr_arg_t arg10         /* argument 10 to pass to entryPt */
    )             