using RabbitMQ.Client.Events;
using RabbitMQ.Queues;
using RabbitMQ.Subscriber;
using System.Text;

namespace Consumer
{
    internal class Program
    {
        private static Subscriber consumer;

        private static void Main(string[] args)
        {
            var selected = false;
            var option = 1;

            consumer = new Subscriber("localhost", 5672, Receive);

            try
            {
                Console.WriteLine("Choose a topic to subscribe to: ");
                var (left, top) = Console.GetCursorPosition();

                while (!selected)
                {
                    Console.SetCursorPosition(left, top);
                    Console.WriteLine($"{(option == 1 ? ">   " : "    ")}Information Log");
                    Console.WriteLine($"{(option == 2 ? ">   " : "    ")}Warning Log");
                    Console.WriteLine($"{(option == 3 ? ">   " : "    ")}Error Log");

                    switch (Console.ReadKey(true).Key)
                    {
                        case ConsoleKey.UpArrow:
                            option = option == 1 ? 3 : --option;
                            break;

                        case ConsoleKey.DownArrow:
                            option = option == 3 ? 1 : ++option;
                            break;

                        case ConsoleKey.Enter:
                            selected = true;
                            break;
                    }
                }

                consumer.Subscribe(option == 1 ? Queue.Information : option == 2 ? Queue.Warning : Queue.Error);
                Console.WriteLine("Listening... Press any key to exit");
                Console.ReadLine();
            }
            finally
            {
                consumer.Dispose();
            }
        }

        private static void Receive(object? sender, BasicDeliverEventArgs e)
        {
            try
            {
                Console.WriteLine($"Message: {Encoding.UTF8.GetString(e.Body.ToArray())}");
                consumer.channel.BasicNack(e.DeliveryTag, false, false);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Ocorreu um erro ao processar a mensagem recebida: {(ex.InnerException ?? ex).Message}. Requeuing");
                consumer.channel.BasicNack(e.DeliveryTag, false, true);
            }
        }
    }
}