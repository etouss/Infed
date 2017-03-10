DELETE FROM customer;
--DELETE FROM lineitem;
--DELETE FROM nation;
--DELETE FROM orders;
--DELETE FROM part;
--DELETE FROM partsupp;
DELETE FROM region;
DELETE FROM supplier;

copy customer from '/Users/etouss/Documents/InfEd/tpch-dbgen/customer.tbl2' WITH DELIMITER as '|' NULL as 'NULL';
copy lineitem from '/Users/etouss/Documents/InfEd/tpch-dbgen/lineitem.tbl2m' WITH DELIMITER as '|' NULL as 'NULL';
--copy nation from '/Users/etouss/Documents/InfEd/tpch-dbgen/nation.tbl2' WITH DELIMITER as '|' NULL as 'NULL';
--copy orders from '/Users/etouss/Documents/InfEd/tpch-dbgen/orders.tbl2' WITH DELIMITER as '|' NULL as 'NULL';
--copy part from '/Users/etouss/Documents/InfEd/tpch-dbgen/part.tbl2m' WITH DELIMITER as '|' NULL as 'NULL';
--copy partsupp from '/Users/etouss/Documents/InfEd/tpch-dbgen/partsupp.tbl2' WITH DELIMITER as '|' NULL as 'NULL';
copy region from '/Users/etouss/Documents/InfEd/tpch-dbgen/region.tbl2' WITH DELIMITER as '|' NULL as 'NULL';
copy supplier from '/Users/etouss/Documents/InfEd/tpch-dbgen/supplier.tbl2m' WITH DELIMITER as '|' NULL as 'NULL';



ALTER TABLE lineitem ALTER COLUMN l_partkey DROP NOT NULL;
UPDATE lineitem SET l_partkey = NULL WHERE l_partkey = 6;
ALTER TABLE lineitem ALTER COLUMN l_suppkey DROP NOT NULL;
UPDATE lineitem SET l_suppkey = NULL WHERE l_suppkey = 6;
ALTER TABLE supplier ALTER COLUMN s_nationkey DROP NOT NULL;
UPDATE supplier SET s_nationkey = NULL WHERE s_nationkey = 6;
ALTER TABLE part ALTER COLUMN p_name DROP NOT NULL;
UPDATE part SET p_name = NULL WHERE p_name LIKE '%light%';
