using System;
using System.Collections.Generic;
using System.Threading;
public class MessageQueue<T> : IDisposable
{
    private readonly int _QUEUE_SIZE;
    // a semaphore object is used to notify enqueue happend signal to the dequeue
    // subrouting
    private Semaphore _semaphore;
    // an internal queue data structure holds the messages in a queue
    private Queue<T> _internalQueue;
    // a private object that is used to acquire a lock on it to ensure synchronous
    // execution of some operations in multithreaded environment
    private object _syncLock;
    /**************************************************************************
    * Construct the message queue object with the maximum size limit.         *
    * If no of messages in the queue meets the maximum size of the queue, any *
    * subsequent enqueue will be discareded. i.e. those message will be lost  *
    * until you dequeue any message i.e. provide a room for new message to    *
    * enter the queue.                                                        *
    **************************************************************************/
    public MessageQueue(int queueSize)
    {
        _syncLock = new object();
        _QUEUE_SIZE = queueSize;
        _internalQueue = new Queue<T>(_QUEUE_SIZE);
        _semaphore = new Semaphore(0, _QUEUE_SIZE);
    }
    /***********************************************************************
    * Reset the MessageQueue                                               *
    ***********************************************************************/
    public void Reset()
    {
        // instantiate the semaphore with initial count 0 i.e. the semaphore is
        // entirely woned and there is no room to enter
        _semaphore = new Semaphore(0, _QUEUE_SIZE);
        // clear all existing messages from the message queue
        _internalQueue.Clear();
    }
    /**********************************************************************
    * Enqueue message in to the Message Queue                             *
    **********************************************************************/
    public void EnqueueMessage(T message)
    {
        lock (_syncLock)
        {
            if (_semaphore != null && message != null)
            {
                try
                {
                    // try to provide a room in the semaphore so that DequeueMessage
                    // can enter into it
                    _semaphore.Release();
                    // now enqueue the message in to the internal queue data structure
                    _internalQueue.Enqueue(message);
                }
                catch { }
            }
        }
    }
    /*********************************************************************
    * Dequeue message from the Message Queue                             *
    *********************************************************************/
    public T DequeueMessage()
    {
        // try to acquire a room in the semaphore and sleep until the room is available
        _semaphore.WaitOne();
        // if any room could be acquired, proceed to next step. i.e. dequeue message from 
        // the internal queue and return it
        lock (_syncLock)
        {
            T message = _internalQueue.Dequeue();
            return message;
        }
    }
    
    /********************************************************************
    * Dispose the Message Queue object                                  *
    ********************************************************************/
    public void Dispose()
    {
        // if the semaphore is not null, close it and set it to null
        if (_semaphore != null)
        {
            _semaphore.Close();
            _semaphore = null;
        }
        // clear the items of the internal queue
        _internalQueue.Clear();
    }
}
