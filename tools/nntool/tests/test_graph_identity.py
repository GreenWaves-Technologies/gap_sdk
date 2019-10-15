from graph.graph_identity import GraphIdentity

def test_graph_identity_hash():
    gi_1 = GraphIdentity("test")
    gi_2 = GraphIdentity("test1")
    gi_3 = GraphIdentity("test1")
    assert gi_1.hexdigest != gi_2.hexdigest and gi_1.hexdigest != gi_3.hexdigest
    assert gi_2.hexdigest == gi_3.hexdigest
    gi_3.is_adjusted = True
    assert gi_2.hexdigest != gi_3.hexdigest
