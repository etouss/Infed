EXPLAIN ANALYSE WITH part_view as (SELECT * FROM part WHERE p_name LIKE '%floral%' OR p_name IS NULL),
                     nation_view as (SELECT * FROM nation WHERE n_name = 'FRANCE')
SELECT count(o_orderkey) FROM orders
WHERE NOT EXISTS(SELECT * FROM lineitem,part_view,supplier,nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey = p_partkey
                AND l_suppkey = s_suppkey
                AND s_nationkey = n_nationkey)
AND NOT EXISTS (SELECT * FROM lineitem,part_view,supplier,nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey IS NULL
                AND l_suppkey = s_suppkey
                AND s_nationkey = n_nationkey)
AND NOT EXISTS (SELECT * FROM lineitem,part_view,supplier,nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey IS NULL
                AND l_suppkey IS NULL
                AND s_nationkey = n_nationkey)
AND NOT EXISTS (SELECT * FROM lineitem,part_view,supplier,nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey IS NULL
                AND l_suppkey IS NULL
                AND s_nationkey IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part_view,supplier,nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey IS NULL
                AND l_suppkey = s_suppkey
                AND s_nationkey IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part_view,supplier,nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey = p_partkey
                AND l_suppkey IS NULL
                AND s_nationkey = n_nationkey)
AND NOT EXISTS(SELECT * FROM lineitem,part_view,supplier,nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey = p_partkey
                AND l_suppkey IS NULL
                AND s_nationkey IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part_view,supplier,nation_view
                WHERE l_orderkey = o_orderkey
                AND l_partkey = p_partkey
                AND l_suppkey = s_suppkey
                AND s_nationkey IS NULL);
