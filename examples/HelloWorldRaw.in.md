# Hello World! with queueing multiple jobs

The first example showed nothing that would have required multiple
threads to print _Hello World!_, and also did not mention anything
about when jobs get deleted. Object life span is of course one of the
crucial questions when programming in C++. So of what type is the
value that is returned by `make_job` in the first example? 

The returned object is of type `JobPointer`, which is a
`QSharedPointer` to `Job`. When `make_job` is executed, it allocates a
`Job` that will later execute the C++ lambda function, and then embeds
it into a shared pointer. Shared pointers count references to the
object pointer they are representing, and delete the object when the
last reference to it is destroyed. In a way, they are single-object
garbage collectors. In the example, the new job is immediately handed
over to the queue stream, and no reference to it is kept by
`main()`. This approach is often called "fire-and-forget jobs". 
The queue will process the job and forget about it when it
has been completed. It will then definitely get deleted automatically,
even though the programmer does not necessarily know when exactly. It
could happen (and in the case of ThreadWeaver jobs) commonly does
deeply in the bowels of Qt event handling when the last event holding
a reference to the job gets destroyed. The gist of it is that from the
programmers point of view, it is not necessary to keep a reference to
a job and delete it later. With that in mind, no further memory
management is required in the HelloWorld example, and the program is
complete.

Fire-and-forget jobs are not always the right tool. For example, if
a job is retrieving and parsing some data, the application needs to
access the data once the job is complete. For that, the programmer
could implement a custom job class.

@@snippet(HelloWorldRaw/HelloWorldRaw.cpp,sample-helloworldraw-class,cpp)

The `QDebugJob` class simply prints a message to `qDebug()` when it is
executed. To implement such a custom job class, it is inherited from
`Job`. By overloading the `run()` method, the "payload", the operation
performed by the job, is being defined. The parameters to the run
method are the job as the queue sees it, and the thread that is
executing the job. The first parameter may be surprising. The reason
that there may be a difference between the job that the queue sees and
`this` is that jobs may be decorated, that means wrapped in something
else that waddles and quacks like a job, before being queued. How this
works will be explained later, what is important to keep in mind for
now is not to assume to always find `this` in the queue. 

