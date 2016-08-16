EXPLAIN ANALYSE SELECT o_orderkey FROM orders
WHERE NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey = s_suppkey
                AND s_nationkey = n_nationkey
                AND p_name LIKE '%floral%')
AND NOT EXISTS (SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey IS NULL
                AND l_suppkey = s_suppkey
                AND s_nationkey = n_nationkey
                AND p_name LIKE '%floral%')
AND NOT EXISTS (SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey IS NULL
                AND l_suppkey IS NULL
                AND s_nationkey = n_nationkey
                AND p_name LIKE '%floral%')
AND NOT EXISTS (SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey IS NULL
                AND l_suppkey IS NULL
                AND s_nationkey IS NULL
                AND p_name LIKE '%floral%')
AND NOT EXISTS (SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey IS NULL
                AND l_suppkey IS NULL
                AND s_nationkey IS NULL
                AND p_name IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey IS NULL
                AND l_suppkey = s_suppkey
                AND s_nationkey IS NULL
                AND p_name LIKE '%floral%')
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey IS NULL
                AND l_suppkey = s_suppkey
                AND s_nationkey IS NULL
                AND p_name IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey IS NULL
                AND l_suppkey = s_suppkey
                AND s_nationkey = n_nationkey
                AND p_name  IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey IS NULL
                AND l_suppkey IS NULL
                AND s_nationkey = n_nationkey
                AND p_name IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey IS NULL
                AND s_nationkey = n_nationkey
                AND p_name IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey IS NULL
                AND s_nationkey = n_nationkey
                AND p_name LIKE '%floral%')
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey IS NULL
                AND s_nationkey IS NULL
                AND p_name LIKE '%floral%')
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey IS NULL
                AND s_nationkey IS NULL
                AND p_name IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey = s_suppkey
                AND s_nationkey IS NULL
                AND p_name LIKE '%floral%')
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey = s_suppkey
                AND s_nationkey IS NULL
                AND p_name IS NULL)
AND NOT EXISTS(SELECT * FROM lineitem,part,supplier,nation
                WHERE l_orderkey = o_orderkey
                AND n_name = 'FRANCE'
                AND l_partkey = p_partkey
                AND l_suppkey = s_suppkey
                AND s_nationkey = n_nationkey
                AND p_name IS NULL);
