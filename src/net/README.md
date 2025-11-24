# Certificado SSL

1. Gerar chave privada

openssl genrsa -out server.key 2048

2. Gerar o certificado .pem

openssl req -new -x509 -key server.key -out server.pem -days 365


## Tabela 1: Campos Essenciais do Certificado

| Campo | Nome Técnico | Requisito de Validação | Finalidade Principal |
| :--- | :--- | :--- | :--- |
| **Common Name** | `CN` | **Deve Corresponder** | **Verificação de Identidade (Hostname):** É o campo que o cliente verifica para garantir que está falando com o servidor certo. |

---

## Tabela 2: Campos Administrativos

| Campo | Nome Técnico | Requisito de Validação | Finalidade Principal |
| :--- | :--- | :--- | :--- |
| **Country Name** | `C` | Obrigatório (Formato ISO) | Identificação Geográfica. |
| **State or Province Name** | `ST` | Obrigatório | Identificação Geográfica. |
| **Locality Name (City)** | `L` | Obrigatório | Identificação Geográfica. |
| **Organization Name** | `O` | Obrigatório | Nome da sua empresa/projeto. |
| **Organization Unit Name** | `OU` | Opcional | Departamento (ex: "TI" ou "Desenvolvimento"). |
| **Email Address** | `emailAddress` | Opcional | Contato do administrador. |