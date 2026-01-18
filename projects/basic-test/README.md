# 💡 Basic Test - Teste Básico LED

## 📝 Descrição

Este é um projeto básico de teste para verificar se a placa **NodeMCU v3 CH340** está funcionando corretamente. O código faz o LED integrado da placa piscar em intervalos de 1 segundo.

**Placa utilizada:** NodeMCU v3 CH340 (ESP8266)

---

## 🎯 Objetivo

- Testar a configuração do Arduino IDE
- Verificar se a placa está sendo reconhecida corretamente
- Confirmar o upload de código
- Validar o funcionamento básico do ESP8266

---

## 🔌 Componentes Necessários

| Componente | Quantidade |
|------------|------------|
| NodeMCU v3 CH340 | 1 |
| Cabo USB Micro-B | 1 |

**Nenhum componente externo é necessário!** Este projeto utiliza apenas o LED integrado da placa.

---

## ⚡ Pinagem

### LED Integrado

O LED integrado da NodeMCU está conectado ao **GPIO2** (pino D4 no label da placa).

```
NodeMCU ESP8266
    └─ GPIO2 (D4) → LED Integrado (azul)
```

No código, utilizamos diretamente o número do GPIO:
```cpp
pinMode(2, OUTPUT);  // GPIO2 = LED integrado
```

---

## 🚀 Como Usar

1. **Conecte** a placa NodeMCU ao computador via cabo USB
2. **Abra** o arquivo `basic-test.ino` no Arduino IDE
3. **Selecione** a placa: `Generic ESP8266 Module`
4. **Selecione** a porta COM correta
5. **Faça o upload** do código (Ctrl+U)
6. **Observe** o LED azul integrado piscando a cada 1 segundo

---

## ✅ Resultado Esperado

- LED integrado (azul) liga por 1 segundo
- LED desliga por 1 segundo
- Padrão se repete continuamente

---

## 🔧 Troubleshooting

**LED não pisca:**
- Verifique se o upload foi concluído com sucesso
- Pressione o botão RST na placa
- Verifique se selecionou a porta COM correta

**Erro ao fazer upload:**
- Consulte a seção de troubleshooting no [README principal](../../README.md)

---

## 📚 Aprendizados

Este projeto ensina:
- ✅ Como fazer upload de código para o ESP8266
- ✅ Controle básico de GPIO (saída digital)
- ✅ Uso das funções `digitalWrite()`, `delay()`
- ✅ Localização do LED integrado na placa

---

**Próximo passo:** Após confirmar que este teste funciona, você está pronto para projetos mais avançados! 🎉
