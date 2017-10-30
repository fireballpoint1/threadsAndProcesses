using System;
using System.Threading;
namespace ConsoleApplication1 
{
    public class Program
    {        
        static int __receivedCount = 0;
        // instantiate the message queue object with maximum size limit 1000. i.e. in
        // any case there will not be more than 1000 items in the queue at a time        
        static MessageQueue<object> __messageQueue = new MessageQueue<object>(1000);
        public static void Main()
        {
            // fork a thread that dequeues messages from the message queue and
            // increments the __receivedCount
            Thread dequeueThread = new Thread(new ThreadStart(_doDequeue));
            dequeueThread.Start();
            
            // we will try to enqueue 10000000 messages in side the queue and will
            // excpect to received 100% of them in the dequeue thread.
            int messageCount = 10000000;
            for (int i = 0; i < messageCount; i++)
            {                
                __messageQueue.EnqueueMessage(new object());
                Console.SetCursorPosition(30, 10);
                Console.WriteLine((i + 1).ToString().PadLeft(10) + " of " + 
                    messageCount.ToString());
            }
            
            Console.WriteLine("Press any key...");
            Console.Read();
            int receivedPercent = (int)((__receivedCount / (double)messageCount) * 100);
            Console.WriteLine(
                "{0} out of {1} message received.\nReceived = {2}%\nLost = {3}%",
                __receivedCount, messageCount, receivedPercent, (100 - receivedPercent));
            Console.Read();
            // abort the dequeue thread
            try 
            {
               dequeueThread.Abort();
            }
            catch { }
            // dispose the message queue object
            __messageQueue.Dispose();
        }
        static void _doDequeue()
        {
            while (true)
            {
                object message = __messageQueue.DequeueMessage();
                __receivedCount++;
            }
        }
    }
}
