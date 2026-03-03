-- Штрихкоди номенклатури (з прив'язкою до одиниці виміру).
-- unit_idrref — основна одиниця або одна з додаткових (nomenclature_units) для цієї номенклатури.
-- barcode унікальний глобально і не повинен збігатися з nomenclature.article.
-- Виконати після 06_nomenclature_units.sql.

-- DROP TRIGGER IF EXISTS nomenclature_barcodes_validate_unit ON public.nomenclature_barcodes;
-- DROP TRIGGER IF EXISTS nomenclature_barcodes_validate_barcode ON public.nomenclature_barcodes;
-- DROP FUNCTION IF EXISTS public.nomenclature_barcodes_validate_unit();
-- DROP FUNCTION IF EXISTS public.nomenclature_barcodes_validate_barcode();
-- DROP TABLE IF EXISTS public.nomenclature_barcodes;

CREATE TABLE IF NOT EXISTS public.nomenclature_barcodes
(
    nomenclature_idrref bytea NOT NULL,
    unit_idrref bytea NOT NULL,
    barcode character varying(64) COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT nomenclature_barcodes_pkey PRIMARY KEY (nomenclature_idrref, unit_idrref, barcode),
    CONSTRAINT nomenclature_barcodes_nomenclature_fkey FOREIGN KEY (nomenclature_idrref)
        REFERENCES public.nomenclature (idrref) ON DELETE CASCADE,
    CONSTRAINT nomenclature_barcodes_unit_fkey FOREIGN KEY (unit_idrref)
        REFERENCES public.units (idrref) ON DELETE RESTRICT
)
TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.nomenclature_barcodes
    OWNER TO postgres;

CREATE UNIQUE INDEX IF NOT EXISTS nomenclature_barcodes_barcode_unique
    ON public.nomenclature_barcodes USING btree (barcode COLLATE pg_catalog."default" ASC NULLS LAST);

-- Перевірка: unit_idrref — основна одиниця номенклатури або з nomenclature_units для цієї номенклатури.
CREATE OR REPLACE FUNCTION public.nomenclature_barcodes_validate_unit()
RETURNS TRIGGER
LANGUAGE plpgsql
AS $$
BEGIN
    IF EXISTS (
        SELECT 1 FROM public.nomenclature n
        WHERE n.idrref = NEW.nomenclature_idrref
          AND n.folder = true
    ) THEN
        RAISE EXCEPTION 'Barcodes are forbidden for folder nomenclature.';
    END IF;

    IF NOT EXISTS (
        SELECT 1 FROM public.nomenclature n
        WHERE n.idrref = NEW.nomenclature_idrref AND n.unit_idrref = NEW.unit_idrref
        UNION
        SELECT 1 FROM public.nomenclature_units nu
        WHERE nu.nomenclature_idrref = NEW.nomenclature_idrref AND nu.unit_idrref = NEW.unit_idrref
    ) THEN
        RAISE EXCEPTION 'unit_idrref must be main unit or additional unit of this nomenclature.';
    END IF;
    RETURN NEW;
END;
$$;

CREATE TRIGGER nomenclature_barcodes_validate_unit
    BEFORE INSERT OR UPDATE
    ON public.nomenclature_barcodes
    FOR EACH ROW
    EXECUTE FUNCTION public.nomenclature_barcodes_validate_unit();

-- Перевірка: barcode не повинен збігатися з жодним nomenclature.article.
CREATE OR REPLACE FUNCTION public.nomenclature_barcodes_validate_barcode()
RETURNS TRIGGER
LANGUAGE plpgsql
AS $$
BEGIN
    IF EXISTS (
        SELECT 1 FROM public.nomenclature n
        WHERE n.article IS NOT NULL AND n.article <> '' AND n.article = NEW.barcode
    ) THEN
        RAISE EXCEPTION 'barcode must not equal any nomenclature.article.';
    END IF;
    RETURN NEW;
END;
$$;

CREATE TRIGGER nomenclature_barcodes_validate_barcode
    BEFORE INSERT OR UPDATE
    ON public.nomenclature_barcodes
    FOR EACH ROW
    EXECUTE FUNCTION public.nomenclature_barcodes_validate_barcode();
