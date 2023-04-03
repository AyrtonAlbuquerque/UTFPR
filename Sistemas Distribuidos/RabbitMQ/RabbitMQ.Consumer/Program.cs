using RabbitMQ.Queues;

namespace RabbitMQ.Consumer
{
    class Program
    {
        static void Main(string[] args)
        {
            using (var consumer = new Consumer("localhost", 5672))
            {
                Console.WriteLine("Choose a queue to subscribe to: ");
                consumer.Subscribe(Queue.First);
                Console.ReadLine();
            }
        }
    }
}