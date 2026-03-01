-- Довідник одиниць виміру (не ієрархічний).
-- Виконати після 01_users.sql (не залежить від users).

-- DROP TABLE IF EXISTS public.units;

CREATE TABLE IF NOT EXISTS public.units
(
    idrref bytea NOT NULL,
    version integer NOT NULL DEFAULT 0,
    marked boolean NOT NULL,
    code character varying(4) COLLATE pg_catalog."default" NOT NULL,
    description character varying(45) COLLATE pg_catalog."default" NOT NULL,
    full_description character varying(100) COLLATE pg_catalog."default" NOT NULL
)
WITH (
    autovacuum_enabled = TRUE,
    toast.autovacuum_enabled = TRUE
)
TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.units
    OWNER TO postgres;

ALTER TABLE IF EXISTS public.units
    ALTER COLUMN idrref SET STORAGE PLAIN;

CREATE UNIQUE INDEX IF NOT EXISTS units_code
    ON public.units USING btree
    (code COLLATE pg_catalog."default" ASC NULLS LAST)
    TABLESPACE pg_default;

CREATE UNIQUE INDEX IF NOT EXISTS units_descr
    ON public.units USING btree
    (description COLLATE pg_catalog."default" ASC NULLS LAST, idrref ASC NULLS LAST)
    TABLESPACE pg_default;

CREATE UNIQUE INDEX IF NOT EXISTS unitshpk
    ON public.units USING btree
    (idrref ASC NULLS LAST)
    TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.units
    CLUSTER ON unitshpk;
