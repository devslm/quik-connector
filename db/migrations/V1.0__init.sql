CREATE TABLE IF NOT EXISTS orders (
    order_num BIGINT NOT NULL PRIMARY KEY,
    flags DECIMAL(12, 6) NOT NULL,
    broker_ref VARCHAR(64) NOT NULL,
    user_id VARCHAR(64) NOT NULL,
    firm_id VARCHAR(64) NOT NULL,
    account VARCHAR(128) NOT NULL,
    price DECIMAL(12, 6) NOT NULL,
    qty DECIMAL(12, 6) NOT NULL,
    balance DECIMAL(12, 6) NOT NULL,
    value DECIMAL(12, 6) NOT NULL,
    accrued_int DECIMAL(12, 6) NOT NULL,
    yield DECIMAL(12, 6) NOT NULL,
    trans_id BIGINT NOT NULL,
    client_code VARCHAR(128) NOT NULL,
    price2 DECIMAL(12, 6) NOT NULL,
    settle_code VARCHAR(64) NOT NULL,
    uid BIGINT NOT NULL,
    canceled_uid BIGINT NOT NULL,
    exchange_code VARCHAR(64) NOT NULL,
    activation_time DECIMAL(12, 6) NOT NULL,
    linked_order BIGINT NOT NULL,
    expiry DECIMAL(12, 6) NOT NULL,
    ticker VARCHAR(8) NOT NULL,
    name VARCHAR(64) NOT NULL,
    class_code VARCHAR(16) NOT NULL,
    class_type VARCHAR(16) NOT NULL,
    status VARCHAR(64) NOT NULL,
    type VARCHAR(64) NOT NULL,
    currency VARCHAR(8) NOT NULL,
    lot_size DECIMAL(12, 6) NOT NULL,
    date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    withdraw_date TIMESTAMP NOT NULL,
    bank_acc_id VARCHAR(128) NOT NULL,
    value_entry_type BIGINT NOT NULL,
    repo_term DECIMAL(12, 6) NOT NULL,
    repo_value DECIMAL(12, 6) NOT NULL,
    repo2value DECIMAL(12, 6) NOT NULL,
    repo_value_balance DECIMAL(12, 6) NOT NULL,
    start_discount DECIMAL(12, 6) NOT NULL,
    reject_reason VARCHAR(512) NOT NULL,
    ext_order_flags DECIMAL(12, 6) NOT NULL,
    min_qty DECIMAL(12, 6) NOT NULL,
    exec_type BIGINT NOT NULL,
    side_qualifier DECIMAL(12, 6) NOT NULL,
    acnt_type DECIMAL(12, 6) NOT NULL,
    capacity DECIMAL(12, 6) NOT NULL,
    passive_only_order DECIMAL(12, 6) NOT NULL,
    visible DECIMAL(12, 6) NOT NULL,
    commission_broker DECIMAL(9, 6) NOT NULL,
    commission_clearing DECIMAL(9, 6) NOT NULL,
    commission_tech_center DECIMAL(9, 6) NOT NULL,
    commission_exchange DECIMAL(9, 6) NOT NULL
);
