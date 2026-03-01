-- Cleanup тестових даних, доданих скриптом 08_seed_nomenclature_50_hierarchy.sql
-- Видаляє:
--   - номенклатуру з code LIKE 'TST%'
--   - тестові одиниці виміру з code IN ('Z901','Z902','Z903','Z904','Z905')
--
-- Порядок важливий через FK:
--   1) barcodes / additional units для тестових unit
--   2) nomenclature TST*
--   3) units Z90*

BEGIN;

WITH target_units AS (
    SELECT u.idrref
    FROM public.units u
    WHERE u.code IN ('Z901', 'Z902', 'Z903', 'Z904', 'Z905')
)
DELETE FROM public.nomenclature_barcodes nb
USING target_units tu
WHERE nb.unit_idrref = tu.idrref;

WITH target_units AS (
    SELECT u.idrref
    FROM public.units u
    WHERE u.code IN ('Z901', 'Z902', 'Z903', 'Z904', 'Z905')
)
DELETE FROM public.nomenclature_units nu
USING target_units tu
WHERE nu.unit_idrref = tu.idrref;

DELETE FROM public.nomenclature n
WHERE n.code LIKE 'TST%';

DELETE FROM public.units u
WHERE u.code IN ('Z901', 'Z902', 'Z903', 'Z904', 'Z905')
  AND NOT EXISTS (
      SELECT 1
      FROM public.nomenclature n
      WHERE n.unit_idrref = u.idrref
  )
  AND NOT EXISTS (
      SELECT 1
      FROM public.nomenclature_units nu
      WHERE nu.unit_idrref = u.idrref
  )
  AND NOT EXISTS (
      SELECT 1
      FROM public.nomenclature_barcodes nb
      WHERE nb.unit_idrref = u.idrref
  );

COMMIT;
