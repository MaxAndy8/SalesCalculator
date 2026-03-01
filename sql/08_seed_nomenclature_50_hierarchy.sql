-- Тестове заповнення довідника номенклатури: 50 записів з ієрархією.
-- Структура:
--   5 кореневих груп
--   10 підгруп (по 2 в кожній кореневій)
--   35 товарів
--
-- Особливість схеми: parent_idrref NOT NULL.
-- Для кореневих груп використовуємо self-parent: parent_idrref = idrref.
--
-- Повторний запуск:
--   - не створює дублікати по idrref (ON CONFLICT DO NOTHING)
--   - за потреби очистки перед запуском:
--       DELETE FROM public.nomenclature WHERE code LIKE 'TST%';
--       DELETE FROM public.units WHERE code IN ('Z901','Z902','Z903','Z904','Z905');

BEGIN;

-- 1) Тестові одиниці виміру (5 шт)
WITH units_seed AS (
    SELECT 'U1'::text AS unit_key, 'Z901'::varchar(4) AS code, 'Штука'::varchar(45) AS description, 'Штука'::varchar(100) AS full_description
    UNION ALL SELECT 'U2', 'Z902', 'Кілограм', 'Кілограм'
    UNION ALL SELECT 'U3', 'Z903', 'Літр', 'Літр'
    UNION ALL SELECT 'U4', 'Z904', 'Метр', 'Метр'
    UNION ALL SELECT 'U5', 'Z905', 'Упаковка', 'Упаковка'
)
INSERT INTO public.units
(
    idrref,
    version,
    marked,
    code,
    description,
    full_description
)
SELECT
    decode(md5(unit_key), 'hex') AS idrref,
    0 AS version,
    false AS marked,
    code,
    description,
    full_description
FROM units_seed
ON CONFLICT (idrref) DO NOTHING;

-- 2) Номенклатура (50 записів)
WITH
groups AS (
    SELECT
        'G' || i::text AS key_id,
        NULL::text AS parent_key,
        NULL::text AS unit_key,
        true AS folder,
        format('TSTG%03s', i) AS code,
        format('Тестова група %s', i) AS description,
        NULL::varchar(25) AS article
    FROM generate_series(1, 5) AS i
),
subgroups AS (
    SELECT
        'S' || i::text AS key_id,
        'G' || (((i - 1) / 2) + 1)::text AS parent_key,
        NULL::text AS unit_key,
        true AS folder,
        format('TSTS%03s', i) AS code,
        format('Тестова підгрупа %s', i) AS description,
        NULL::varchar(25) AS article
    FROM generate_series(1, 10) AS i
),
items AS (
    SELECT
        'I' || i::text AS key_id,
        'S' || (((i - 1) % 10) + 1)::text AS parent_key,
        'U' || (((i - 1) % 5) + 1)::text AS unit_key,
        false AS folder,
        format('TSTI%03s', i) AS code,
        format('Тестовий товар %s', i) AS description,
        format('ART-TST-%03s', i) AS article
    FROM generate_series(1, 35) AS i
),
all_rows AS (
    SELECT * FROM groups
    UNION ALL
    SELECT * FROM subgroups
    UNION ALL
    SELECT * FROM items
),
resolved AS (
    SELECT
        decode(md5(r.key_id), 'hex') AS idrref,
        CASE
            WHEN r.parent_key IS NULL THEN decode(md5(r.key_id), 'hex')
            ELSE decode(md5(r.parent_key), 'hex')
        END AS parent_idrref,
        CASE
            WHEN r.unit_key IS NULL THEN NULL::bytea
            ELSE decode(md5(r.unit_key), 'hex')
        END AS unit_idrref,
        r.folder,
        r.code,
        r.description,
        r.article
    FROM all_rows r
)
INSERT INTO public.nomenclature
(
    idrref,
    version,
    marked,
    parent_idrref,
    folder,
    code,
    description,
    full_description,
    article,
    unit_idrref,
    service
)
SELECT
    idrref,
    0 AS version,
    false AS marked,
    parent_idrref,
    folder,
    code,
    description,
    description || ' (тестові дані)' AS full_description,
    article,
    unit_idrref,
    false AS service
FROM resolved
ON CONFLICT (idrref) DO NOTHING;

COMMIT;
