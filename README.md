## Параметры биржевой информации

/// STRING Полное название бумаги
LONGNAME,

/// STRING Краткое название бумаги
SHORTNAME,

/// STRING Код бумаги
CODE,

/// STRING Название класса
CLASSNAME,

/// STRING Код класса
CLASS_CODE,

/// STRING Дата торгов
TRADE_DATE_CODE,

/// STRING Дата погашения
MAT_DATE,

/// Int Число дней до погашения
DAYS_TO_MAT_DATE,

/// Decimal Номинал бумаги
SEC_FACE_VALUE,

/// STRING Валюта номинала
SEC_FACE_UNIT,

/// Int Точность цены
SEC_SCALE,

/// Decimal Минимальный шаг цены
SEC_PRICE_STEP,

/// STRING Тип инструмента
SECTYPE,

/// Int Статус
STATUS,

/// Decimal Размер лота
LOTSIZE,

/// Decimal Лучшая цена спроса
BID,

/// Decimal Спрос по лучшей цене
BIDDEPTH,

/// Decimal Суммарный спрос
BIDDEPTHT,

/// Long Количество заявок на покупку
NUMBIDS,

/// Decimal Лучшая цена предложения
OFFER,

/// Decimal Предложение по лучшей цене
OFFERDEPTH,

/// Decimal Суммарное предложение
OFFERDEPTHT,

/// Long Количество заявок на продажу
NUMOFFERS,

/// Decimal Цена открытия
OPEN,

/// Decimal Максимальная цена сделки
HIGH,

/// Decimal Минимальная цена сделки
LOW,

/// Decimal Цена последней сделки
LAST,

/// Decimal Разница цены последней к предыдущей сессии
CHANGE,

/// Long Количество бумаг в последней сделке
QTY,

/// STRING Время последней сделки
TIME,

/// Long Количество бумаг в обезличенных сделках
VOLTODAY,

/// Long Оборот в деньгах
VALTODAY,

/// Int Состояние сессии
TRADINGSTATUS,

/// Decimal Оборот в деньгах последней сделки
VALUE,

/// Decimal Средневзвешенная цена
WAPRICE,

/// Decimal Лучшая цена спроса сегодня
HIGHBID,

/// Decimal Лучшая цена предложения сегодня
LOWOFFER,

/// Long Количество сделок за сегодня
NUMTRADES,

/// Decimal Цена закрытия
PREVPRICE,

/// Decimal Предыдущая оценка
PREVWAPRICE,

/// Decimal Цена периода закрытия
CLOSEPRICE,

/// Decimal % изменения от закрытия
LASTCHANGE,

/// STRING Размещение
PRIMARYDIST,

/// Decimal Накопленный купонный доход
ACCRUEDINT,

/// Decimal Доходность последней сделки
YIELD,

/// Decimal Размер купона
COUPONVALUE,

/// Decimal Доходность по предыдущей оценке
YIELDATPREVWAPRICE,

/// Decimal Доходность по оценке
YIELDATWAPRICE,

/// Decimal Разница цены последней к предыдущей оценке
PRICEMINUSPREVWAPRICE,

/// Decimal Доходность закрытия
CLOSEYIELD,

/// Decimal Текущее значение индексов Московской Биржи
CURRENTVALUE,

/// Decimal Значение индексов Московской Биржи на закрытие предыдущего дня
LASTVALUE,

/// Decimal Разница цены последней к предыдущей сессии
LASTTOPREVSTLPRC,

/// Decimal Предыдущая расчетная цена
PREVSETTLEPRICE,

/// Decimal Лимит изменения цены
PRICEMVTLIMIT,

/// NUMERIC Лимит изменения цены T1
PRICEMVTLIMITT1,

/// NUMERIC Лимит объема активных заявок(в контрактах)
MAXOUTVOLUME,

/// NUMERIC Максимально возможная цена
PRICEMAX,

/// NUMERIC Минимально возможная цена
PRICEMIN,

/// NUMERIC Оборот внесистемных в деньгах
NEGVALTODAY,

/// NUMERIC Количество внесистемных сделок за сегодня
NEGNUMTRADES,

/// NUMERIC Количество открытых позиций
NUMCONTRACTS,

/// STRING Время закрытия предыдущих торгов(для индексов РТС)
CLOSETIME,

/// NUMERIC Значение индекса РТС на момент открытия торгов
OPENVAL,

/// NUMERIC Изменение текущего индекса РТС по сравнению со значением открытия
CHNGOPEN,

/// NUMERIC Изменение текущего индекса РТС по сравнению со значением закрытия
CHNGCLOSE,

/// NUMERIC Гарантийное обеспечение продавца
BUYDEPO,

/// NUMERIC Гарантийное обеспечение покупателя
SELLDEPO,

/// STRING Время последнего изменения
CHANGETIME,

/// NUMERIC Доходность продажи
SELLPROFIT,

/// NUMERIC Доходность покупки
BUYPROFIT,

/// NUMERIC Разница цены последней к предыдущей сделки(FORTS, ФБ СПБ, СПВБ)
TRADECHANGE,

/// NUMERIC Номинал(для бумаг СПВБ)
FACEVALUE,

/// NUMERIC Рыночная цена вчера
MARKETPRICE,

/// NUMERIC Рыночная цена
MARKETPRICETODAY,

/// NUMERIC Дата выплаты купона
NEXTCOUPON,

/// NUMERIC Цена оферты
BUYBACKPRICE,

/// NUMERIC Дата оферты
BUYBACKDATE,

/// NUMERIC Объем обращения
ISSUESIZE,

/// NUMERIC Дата предыдущего торгового дня
PREVDATE,

/// NUMERIC Дюрация
DURATION,

/// NUMERIC Официальная цена открытия
LOPENPRICE,

/// NUMERIC Официальная текущая цена
LCURRENTPRICE,

/// NUMERIC Официальная цена закрытия
LCLOSEPRICE,

/// STRING Тип цены
QUOTEBASIS,

/// NUMERIC Признаваемая котировка предыдущего дня
PREVADMITTEDQUOT,

/// NUMERIC Лучшая спрос на момент завершения периода торгов
LASTBID,

/// NUMERIC Лучшее предложение на момент завершения торгов
LASTOFFER,

/// NUMERIC Цена закрытия предыдущего дня
PREVLEGALCLOSEPR,

/// NUMERIC Длительность купона
COUPONPERIOD,

/// NUMERIC Рыночная цена 2
MARKETPRICE2,

/// NUMERIC Признаваемая котировка
ADMITTEDQUOTE,

/// NUMERIC БГО по покрытым позициям
BGOP,

/// NUMERIC БГО по непокрытым позициям
BGONP,

/// NUMERIC Цена страйк
STRIKE,

/// NUMERIC Стоимость шага цены
STEPPRICET,

/// NUMERIC Стоимость шага цены(для новых контрактов FORTS и RTS Standard)
STEPPRICE,

/// NUMERIC Расчетная цена
SETTLEPRICE,

/// STRING Тип опциона
OPTIONTYPE,

/// STRING Базовый актив
OPTIONBASE,

/// NUMERIC Волатильность опциона
VOLATILITY,

/// NUMERIC Теоретическая цена
THEORPRICE,

/// NUMERIC  Агрегированная ставка
PERCENTRATE,

/// Int Тип цены фьючерса
ISPERCENT,

/// Int Статус клиринга
CLSTATE,

/// Decimal Котировка последнего клиринга
CLPRICE,

/// STRING Начало основной сессии
STARTTIME,

/// STRING Окончание основной сессии
ENDTIME,

/// STRING Начало вечерней сессии
EVNSTARTTIME,

/// STRING Окончание вечерней сессии
EVNENDTIME,

/// STRING Начало утренней сессии
MONSTARTTIME,

/// STRING Окончание утренней сессии
MONENDTIME,

/// STRING Валюта шага цены
CURSTEPPRICE,

/// NUMERIC  Текущая рыночная котировка
REALVMPRICE,

/// STRING Маржируемый
MARG,

/// NUMERIC Дата исполнения инструмента
EXPDATE,

/// NUMERIC Курс
CROSSRATE,

/// NUMERIC Базовый курс
BASEPRICE,

/// NUMERIC Максимальное значение(RTSIND)
HIGHVAL,

/// NUMERIC Минимальное значение(RTSIND)
LOWVAL,

/// NUMERIC Изменение(RTSIND)
ICHANGE,

/// NUMERIC Значение на момент открытия(RTSIND)
IOPEN,

/// NUMERIC Процент изменения(RTSIND)
PCHANGE,

/// NUMERIC Цена предторгового периода
OPENPERIODPRICE,

/// NUMERIC Минимальная текущая цена
MIN_CURR_LAST,

/// STRING Код расчетов по умолчанию
SETTLECODE,

/// DOUBLE Стоимость шага цены для клиринга
STEPPRICECL,

/// DOUBLE Стоимость шага цены для промклиринга
STEPPRICEPRCL,

/// STRING Время изменения минимальной текущей цены
MIN_CURR_LAST_TI,

/// DOUBLE Предыдущее значение размера лота
PREVLOTSIZE,

/// DOUBLE Дата последнего изменения размера лота
LOTSIZECHANGEDAT,

/// NUMERIC Цена послеторгового аукциона
AUCTPRICE,

/// NUMERIC Количество в сделках послеторгового аукциона
CLOSING_AUCTION_VOLUME