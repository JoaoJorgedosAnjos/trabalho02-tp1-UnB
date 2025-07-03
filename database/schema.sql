-- Schema SQLite para Sistema de Investimentos
-- Arquivo: database/schema.sql

-- =============================================
-- TABELA: contas
-- Armazena dados dos usuários do sistema
-- =============================================
CREATE TABLE IF NOT EXISTS contas (
    cpf TEXT PRIMARY KEY,              -- CPF no formato XXX.XXX.XXX-XX
    nome TEXT NOT NULL,                -- Nome do usuário (máx 20 chars)
    senha TEXT NOT NULL,               -- Senha com validação de força
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- =============================================
-- TABELA: carteiras 
-- Armazena carteiras de investimento dos usuários
-- =============================================
CREATE TABLE IF NOT EXISTS carteiras (
    codigo TEXT PRIMARY KEY,           -- Código único da carteira (5 dígitos)
    nome TEXT NOT NULL,                -- Nome da carteira (máx 20 chars)
    tipo_perfil TEXT NOT NULL,         -- "Conservador", "Moderado", "Agressivo"
    cpf_conta TEXT NOT NULL,           -- CPF do proprietário
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    
    FOREIGN KEY (cpf_conta) REFERENCES contas(cpf) ON DELETE CASCADE,
    CHECK (tipo_perfil IN ('Conservador', 'Moderado', 'Agressivo'))
);

-- =============================================
-- TABELA: ordens
-- Armazena ordens de investimento das carteiras
-- =============================================
CREATE TABLE IF NOT EXISTS ordens (
    codigo TEXT PRIMARY KEY,           -- Código único da ordem (5 dígitos)
    codigo_neg TEXT NOT NULL,          -- Código de negociação (12 chars)
    data TEXT NOT NULL,                -- Data no formato AAAAMMDD
    valor TEXT NOT NULL,               -- Valor no formato brasileiro (R$ X.XXX,XX)
    quantidade TEXT NOT NULL,          -- Quantidade de papéis
    codigo_carteira TEXT NOT NULL,     -- Código da carteira proprietária
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    
    FOREIGN KEY (codigo_carteira) REFERENCES carteiras(codigo) ON DELETE CASCADE
);

-- =============================================
-- ÍNDICES PARA PERFORMANCE
-- =============================================
CREATE INDEX IF NOT EXISTS idx_carteiras_cpf ON carteiras(cpf_conta);
CREATE INDEX IF NOT EXISTS idx_ordens_carteira ON ordens(codigo_carteira);
CREATE INDEX IF NOT EXISTS idx_ordens_data ON ordens(data);
CREATE INDEX IF NOT EXISTS idx_ordens_codigo_neg ON ordens(codigo_neg);

-- =============================================
-- TRIGGERS PARA ATUALIZAR updated_at
-- =============================================
CREATE TRIGGER IF NOT EXISTS update_contas_timestamp 
    AFTER UPDATE ON contas
BEGIN
    UPDATE contas SET updated_at = CURRENT_TIMESTAMP WHERE cpf = NEW.cpf;
END;

CREATE TRIGGER IF NOT EXISTS update_carteiras_timestamp 
    AFTER UPDATE ON carteiras
BEGIN
    UPDATE carteiras SET updated_at = CURRENT_TIMESTAMP WHERE codigo = NEW.codigo;
END;

CREATE TRIGGER IF NOT EXISTS update_ordens_timestamp 
    AFTER UPDATE ON ordens
BEGIN
    UPDATE ordens SET updated_at = CURRENT_TIMESTAMP WHERE codigo = NEW.codigo;
END; 