#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <vector>

// --- Configuração Wi-Fi (Access Point) ---
const char* ssid = "SCRM-12";
const char* password = "12345678";

WebServer server(80);

// --- Pinos e variáveis ---
const int pinHumiditySensor = 34;
const int led = 25;
unsigned long ultimoLeitura = 0;
const unsigned long intervaloLeitura = 30UL * 60UL * 1000UL; // 30 minutos

struct Registro {
  String dataHora;
  int umidade;
};

std::vector<Registro> historico;

// --- Função para pegar data/hora ---
String getDataHora() {
  time_t agora = time(nullptr);
  struct tm* t = localtime(&agora);
  char buffer[25];
  strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", t);
  return String(buffer);
}

// --- Página principal ---
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f4f6f8; text-align: center; }";
  html += "h2 { color: #003366; font-size: 28px; margin-top: 20px; }";
  html += "h3 { color: #333; font-size: 18px; margin: 5px 0; }";
  html += ".resumo { display: inline-block; background: #fff; border-radius: 15px; padding: 20px 40px; margin-top: 25px; box-shadow: 2px 2px 10px rgba(0,0,0,0.1); }";
  html += ".linha { padding: 10px 20px; border-radius: 8px; margin: 10px 0; font-size: 18px; font-weight: bold; color: #fff; }";
  html += ".hora { background-color: #0077cc; }";
  html += ".vinte { background-color: #00b386; }";
  html += ".cinco { background-color: #e67e22; }";
  html += "a { display: inline-block; margin-top: 25px; font-size: 18px; text-decoration: none; color: #0066cc; }";
  html += "a:hover { text-decoration: underline; }";
  html += "</style></head><body>";
  html += 
  html += "<h2>Sensor Contador de Acessos<br> Em Teste</h2>";
  html += "<h3>Responsáveis: Caua e Thalia - 3DSTB 14</h3>";
  html += "<h3>Orientador: Prof. Israel</h3>";
  html += "<title>Sensor de Umidade</title>";
  html += "<style>";
  html += "body { font-family: Arial; background-color: #f4f6f8; text-align: center; }";
  html += "table { margin: 20px auto; border-collapse: collapse; width: 80%; }";
  html += "th, td { border: 1px solid #ccc; padding: 8px; }";
  html += "th { background-color: #003366; color: white; }";
  html += "button { background-color: #007bff; color: white; padding: 10px 20px; border: none; cursor: pointer; border-radius: 5px; }";
  html += "button:hover { background-color: #0056b3; }";
  html += "</style></head><body>";
  html += "<h2>Histórico de Umidade do Solo</h2>";

  html += "<table><tr><th>Data e Hora</th><th>Umidade (valor bruto)</th></tr>";
  for (int i = historico.size() - 1; i >= 0; i--) {
    html += "<tr><td>" + historico[i].dataHora + "</td><td>" + String(historico[i].umidade) + "</td></tr>";
  }
  html += "</table>";

  html += "<a href='/download'><button>📥 Download CSV</button></a>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// --- Gera o CSV ---
void handleDownload() {
  String csv = "Data e Hora,Umidade\n";
  for (const auto& reg : historico) {
    csv += reg.dataHora + "," + String(reg.umidade) + "\n";
  }
  server.send(200, "text/csv", csv);
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  pinMode(pinHumiditySensor, INPUT);
  pinMode(led, OUTPUT);

  // Inicializa relógio com data da compilação
  struct tm tm_build = {0};
  strptime(__DATE__ " " __TIME__, "%b %d %Y %H:%M:%S", &tm_build);
  time_t t = mktime(&tm_build);
  struct timeval now = { .tv_sec = t };
  settimeofday(&now, NULL);

  WiFi.softAP(ssid, password);
  Serial.println("Access Point iniciado.");
  Serial.print("SSID: "); Serial.println(ssid);
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/download", handleDownload);
  server.begin();
}

// --- Loop ---
void loop() {
  unsigned long agora = millis();

  if (agora - ultimoLeitura >= intervaloLeitura || historico.empty()) {
    ultimoLeitura = agora;

    int valorUmidade = analogRead(pinHumiditySensor);
    Serial.print("Leitura: "); Serial.println(valorUmidade);

    if (valorUmidade >= 4000) {
      digitalWrite(led, HIGH);
      delay(6000);
      digitalWrite(led, LOW);
      Serial.println("Solo seco! LED piscou 6s.");
    }

    Registro reg;
    reg.dataHora = getDataHora();
    reg.umidade = valorUmidade;
    historico.push_back(reg);
  }

  server.handleClient();
}
