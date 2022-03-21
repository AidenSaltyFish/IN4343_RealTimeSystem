TASK_ID taskSpawn
    (
    char *        name,        /* name of new task */
    int           priority,    /* priority of new task */
    int           options,     /* task option word */
    size_t        stackSize,   /* size (bytes) of stack needed plus name */
    FUNCPTR       entryPt,     /* entry point of new task */
    _Vx_usr_arg_t arg1,        /* argument 1 to pass to entryPt */
    ..., 
    _Vx_usr_arg_t arg10        /* argument 10 to pass to entryPt */
    )      