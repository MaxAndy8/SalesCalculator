-- Таблиця користувачів (структура з проєкту).
-- Паролі зберігаються у відкритому вигляді (bytea UTF-8) на даному етапі.

-- DROP TABLE IF EXISTS public.users;

CREATE TABLE IF NOT EXISTS public.users
(
    idrref bytea NOT NULL,
    version integer NOT NULL DEFAULT 0,
    marked boolean NOT NULL,
    description character varying(45) COLLATE pg_catalog."default" NOT NULL,
    full_description character varying(100) COLLATE pg_catalog."default" NOT NULL,
    "show" boolean NOT NULL,
    password bytea NOT NULL
)
WITH (
    autovacuum_enabled = TRUE,
    toast.autovacuum_enabled = TRUE
)
TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.users
    OWNER TO postgres;

ALTER TABLE IF EXISTS public.users
    ALTER COLUMN idrref SET STORAGE PLAIN;

CREATE UNIQUE INDEX IF NOT EXISTS users_descr
    ON public.users USING btree
    (description COLLATE pg_catalog."default" ASC NULLS LAST, idrref ASC NULLS LAST)
    TABLESPACE pg_default;

CREATE UNIQUE INDEX IF NOT EXISTS usershpk
    ON public.users USING btree
    (idrref ASC NULLS LAST)
    TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.users
    CLUSTER ON usershpk;
