using RabbitMQ.Client;
using RabbitMQ.Client.Events;

namespace RabbitMQ.Subscriber
{
    public class Subscriber : IDisposable
    {
        public readonly IModel channel;
        private IConnection _connection;
        private EventingBasicConsumer _consumer;

        public Subscriber(string host, int port, EventHandler<BasicDeliverEventArgs> receive)
        {
            if (string.IsNullOrEmpty(host))
            {
                throw new Exception("O IP do host precisa ser informado");
            }

            if (port < 0 || port > 65535)
            {
                throw new Exception($"O valor da porta {port} é inválido");
            }

            var factory = new ConnectionFactory
            {
                HostName = host,
                VirtualHost = "/",
                Port = port,
                UserName = "guest",
                Password = "guest"
            };
            _connection = factory.CreateConnection();
            channel = _connection.CreateModel();
            _consumer = new EventingBasicConsumer(channel);
            _consumer.Received += receive;
        }

        public void Dispose()
        {
            channel.Close();
            _connection.Close();
        }

        public string Subscribe(string queue)
        {
            return channel.BasicConsume(queue, false, _consumer);
        }
    }
}