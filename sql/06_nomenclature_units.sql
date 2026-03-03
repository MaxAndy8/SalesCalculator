-- Додаткові одиниці виміру номенклатури (коефіцієнт до основної одиниці).
-- Виконати після 05_nomenclature.sql та 04_units.sql.

-- DROP TRIGGER IF EXISTS nomenclature_units_forbid_main_unit ON public.nomenclature_units;
-- DROP FUNCTION IF EXISTS public.nomenclature_units_forbid_main_unit();
-- DROP TABLE IF EXISTS public.nomenclature_units;

CREATE TABLE IF NOT EXISTS public.nomenclature_units
(
    nomenclature_idrref bytea NOT NULL,
    unit_idrref bytea NOT NULL,
    coefficient numeric NOT NULL,
    CONSTRAINT nomenclature_units_pkey PRIMARY KEY (nomenclature_idrref, unit_idrref),
    CONSTRAINT nomenclature_units_nomenclature_fkey FOREIGN KEY (nomenclature_idrref)
        REFERENCES public.nomenclature (idrref) ON DELETE CASCADE,
    CONSTRAINT nomenclature_units_unit_fkey FOREIGN KEY (unit_idrref)
        REFERENCES public.units (idrref) ON DELETE RESTRICT,
    CONSTRAINT nomenclature_units_coefficient_positive CHECK (coefficient > 0)
)
TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.nomenclature_units
    OWNER TO postgres;

ALTER TABLE IF EXISTS public.nomenclature_units
    ALTER COLUMN nomenclature_idrref SET STORAGE PLAIN;
ALTER TABLE IF EXISTS public.nomenclature_units
    ALTER COLUMN unit_idrref SET STORAGE PLAIN;

-- Заборона додавати основну одиницю (nomenclature.unit_idrref) в додаткові.
CREATE OR REPLACE FUNCTION public.nomenclature_units_forbid_main_unit()
RETURNS TRIGGER
LANGUAGE plpgsql
AS $$
BEGIN
    IF EXISTS (
        SELECT 1 FROM public.nomenclature n
        WHERE n.idrref = NEW.nomenclature_idrref
          AND n.folder = true
    ) THEN
        RAISE EXCEPTION 'Additional units are forbidden for folder nomenclature.';
    END IF;

    IF EXISTS (
        SELECT 1 FROM public.nomenclature n
        WHERE n.idrref = NEW.nomenclature_idrref
          AND n.unit_idrref IS NOT NULL
          AND n.unit_idrref = NEW.unit_idrref
    ) THEN
        RAISE EXCEPTION 'Main unit of nomenclature cannot be added as additional unit.';
    END IF;
    RETURN NEW;
END;
$$;

CREATE TRIGGER nomenclature_units_forbid_main_unit
    BEFORE INSERT OR UPDATE
    ON public.nomenclature_units
    FOR EACH ROW
    EXECUTE FUNCTION public.nomenclature_units_forbid_main_unit();
