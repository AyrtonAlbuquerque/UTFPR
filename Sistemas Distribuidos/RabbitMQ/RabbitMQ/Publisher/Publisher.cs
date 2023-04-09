using RabbitMQ.Client;
using System.Text;

namespace RabbitMQ.Publisher
{
    public class Publisher : IDisposable
    {
        private IModel _channel;
        private IConnection _connection;

        public Publisher(string host, int port)
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
            _channel = _connection.CreateModel();
        }

        public void Dispose()
        {
            _channel.Close();
            _connection.Close();
        }

        public void Publish(string exchange, string routingKey, string message)
        {
            _channel.BasicPublish(exchange, routingKey, null, Encoding.UTF8.GetBytes(message));
        }
    }
}