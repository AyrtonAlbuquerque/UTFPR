using RabbitMQ.Client;
using RabbitMQ.Client.Events;
using System.Text;

namespace RabbitMQ.Consumer
{
    public class Consumer : IDisposable
    {
        private IConnection _connection;
        private IModel _channel;
        private EventingBasicConsumer _consumer;

        public Consumer(string host, int port)
        {
            if (string.IsNullOrEmpty(host)) throw new Exception("O IP do host precisa ser informado");
            if (port < 0 || port > 65535) throw new Exception($"O valor da porta {port} é inválido");

            var factory = new ConnectionFactory
            {
                HostName = host,
                VirtualHost = "/",
                Port = port,
                UserName = "guest",
                Password = "guest"
            };
            _connection = factory.CreateConnection();
            _channel = _connection.CreateModel();
            _consumer = new EventingBasicConsumer(_channel);
            _consumer.Received += Receive;
        }

        public void Dispose()
        {
            _channel.Close();
            _connection.Close();
        }

        public string Subscribe(string queue)
        {
            return _channel.BasicConsume(queue, false, _consumer);
        }

        private void Receive(object? sender, BasicDeliverEventArgs e)
        {
            try
            {
                Console.WriteLine($"Message: {Encoding.UTF8.GetString(e.Body.ToArray())}");
                _channel.BasicNack(e.DeliveryTag, false, false);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Ocorreu um erro ao processar a mensagem recebida: {(ex.InnerException ?? ex).Message}. Requeuing");
                _channel.BasicNack(e.DeliveryTag, false, true);
            }
        }
    }
}