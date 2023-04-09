using RabbitMQ.Bindings;
using RabbitMQ.Exchanges;
using RabbitMQ.Publisher;

namespace Producer
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            var selected = false;
            var exit = false;
            var option = 1;
            string routingKey;
            var producer = new Publisher("localhost", 5672);

            try
            {
                Console.WriteLine("Choose a topic to publish to: ");
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

                routingKey = option == 1 ? Binding.Information : option == 2 ? Binding.Warning : Binding.Error;

                while (!exit)
                {
                    Console.WriteLine($"Message to {routingKey}: (Type exit to terminate)");
                    var message = Console.ReadLine();

                    if (!string.IsNullOrEmpty(message))
                    {
                        if (message == "exit")
                        {
                            exit = true;
                        }
                        else
                        {
                            producer.Publish(Exchange.Direct, routingKey, message);
                        }
                    }
                }
            }
            finally
            {
                producer.Dispose();
            }
        }
    }
}