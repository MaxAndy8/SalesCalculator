-- Довідник номенклатури (ієрархічний).
-- Поля tax_invoices_default_coderref та type на даній ітерації не реалізуються.
-- Виконати після 04_units.sql (FK unit_idrref -> units.idrref).

-- DROP TRIGGER IF EXISTS check_duplicate_articles_trigger ON public.nomenclature;
-- DROP FUNCTION IF EXISTS public.check_duplicate_articles_trigger();
-- DROP TRIGGER IF EXISTS nomenclature_folder_fields_guard_trigger ON public.nomenclature;
-- DROP FUNCTION IF EXISTS public.nomenclature_folder_fields_guard_trigger();
-- DROP TABLE IF EXISTS public.nomenclature;

CREATE TABLE IF NOT EXISTS public.nomenclature
(
    idrref bytea NOT NULL,
    version integer NOT NULL DEFAULT 0,
    marked boolean NOT NULL,
    parent_idrref bytea,
    folder boolean NOT NULL,
    code character varying(11) COLLATE pg_catalog."default" NOT NULL,
    description character varying(100) COLLATE pg_catalog."default" NOT NULL,
    full_description character varying(1000) COLLATE pg_catalog."default",
    article character varying(25) COLLATE pg_catalog."default",
    unit_idrref bytea,
    service boolean DEFAULT false
)
WITH (
    autovacuum_enabled = TRUE,
    toast.autovacuum_enabled = TRUE
)
TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.nomenclature
    OWNER TO postgres;

ALTER TABLE IF EXISTS public.nomenclature
    ALTER COLUMN idrref SET STORAGE PLAIN;
ALTER TABLE IF EXISTS public.nomenclature
    ALTER COLUMN parent_idrref SET STORAGE PLAIN;
ALTER TABLE IF EXISTS public.nomenclature
    ALTER COLUMN parent_idrref DROP NOT NULL;
ALTER TABLE IF EXISTS public.nomenclature
    ALTER COLUMN unit_idrref SET STORAGE PLAIN;
ALTER TABLE IF EXISTS public.nomenclature
    ALTER COLUMN service DROP NOT NULL;

-- Для folder=true поля article/unit_idrref/service повинні бути NULL.
-- NOT VALID: не перевіряє історичні дані, але блокує нові/оновлені порушення.
DO $$
BEGIN
    IF NOT EXISTS (
        SELECT 1
        FROM pg_constraint
        WHERE conname = 'nomenclature_folder_fields_null_chk'
          AND conrelid = 'public.nomenclature'::regclass
    ) THEN
        ALTER TABLE public.nomenclature
            ADD CONSTRAINT nomenclature_folder_fields_null_chk
            CHECK (
                NOT folder
                OR (article IS NULL AND unit_idrref IS NULL AND service IS NULL)
            ) NOT VALID;
    END IF;
END
$$;

-- Індекси
CREATE UNIQUE INDEX IF NOT EXISTS nomenclature_article
    ON public.nomenclature USING btree (article ASC NULLS LAST, idrref ASC NULLS LAST);

CREATE UNIQUE INDEX IF NOT EXISTS nomenclature_code
    ON public.nomenclature USING btree (code ASC NULLS LAST, idrref ASC NULLS LAST);

CREATE UNIQUE INDEX IF NOT EXISTS nomenclature_descr
    ON public.nomenclature USING btree (description ASC NULLS LAST, idrref ASC NULLS LAST);

CREATE UNIQUE INDEX IF NOT EXISTS nomenclature_idrref
    ON public.nomenclature USING btree (idrref ASC NULLS LAST);

CREATE UNIQUE INDEX IF NOT EXISTS nomenclature_parent_folder_article_id
    ON public.nomenclature USING btree (parent_idrref ASC NULLS LAST, folder ASC NULLS LAST, article ASC NULLS LAST, idrref ASC NULLS LAST);

CREATE UNIQUE INDEX IF NOT EXISTS nomenclature_parent_folder_id
    ON public.nomenclature USING btree (parent_idrref ASC NULLS LAST, folder ASC NULLS LAST, idrref ASC NULLS LAST);

CREATE UNIQUE INDEX IF NOT EXISTS nomenclature_parentcode
    ON public.nomenclature USING btree (parent_idrref ASC NULLS LAST, folder ASC NULLS LAST, code ASC NULLS LAST, idrref ASC NULLS LAST);

CREATE UNIQUE INDEX IF NOT EXISTS nomenclature_parentdescr
    ON public.nomenclature USING btree (parent_idrref ASC NULLS LAST, folder ASC NULLS LAST, description ASC NULLS LAST, idrref ASC NULLS LAST);

CREATE UNIQUE INDEX IF NOT EXISTS nomenclaturehpk
    ON public.nomenclature USING btree (idrref ASC NULLS LAST);

ALTER TABLE IF EXISTS public.nomenclature
    CLUSTER ON nomenclaturehpk;

-- Функція тригера: перевірка унікальності article в межах (parent_idrref, folder).
CREATE OR REPLACE FUNCTION public.check_duplicate_articles_trigger()
RETURNS TRIGGER
LANGUAGE plpgsql
AS $$
BEGIN
    IF NEW.article IS NULL OR NEW.article = '' THEN
        RETURN NEW;
    END IF;
    IF EXISTS (
        SELECT 1 FROM public.nomenclature n
        WHERE n.parent_idrref IS NOT DISTINCT FROM NEW.parent_idrref
          AND n.folder = NEW.folder
          AND n.article = NEW.article
          AND n.idrref IS DISTINCT FROM NEW.idrref
    ) THEN
        RAISE EXCEPTION 'Duplicate article in same parent/folder: %', NEW.article;
    END IF;
    RETURN NEW;
END;
$$;

CREATE TRIGGER check_duplicate_articles_trigger
    BEFORE INSERT OR UPDATE
    ON public.nomenclature
    FOR EACH ROW
    EXECUTE FUNCTION public.check_duplicate_articles_trigger();

-- Тригер-блокування порушень для нових/оновлених рядків папок.
CREATE OR REPLACE FUNCTION public.nomenclature_folder_fields_guard_trigger()
RETURNS TRIGGER
LANGUAGE plpgsql
AS $$
BEGIN
    IF NEW.folder AND (NEW.article IS NOT NULL OR NEW.unit_idrref IS NOT NULL OR NEW.service IS NOT NULL) THEN
        RAISE EXCEPTION 'For folder=true, article, unit_idrref and service must be NULL.';
    END IF;
    RETURN NEW;
END;
$$;

CREATE TRIGGER nomenclature_folder_fields_guard_trigger
    BEFORE INSERT OR UPDATE
    ON public.nomenclature
    FOR EACH ROW
    EXECUTE FUNCTION public.nomenclature_folder_fields_guard_trigger();
