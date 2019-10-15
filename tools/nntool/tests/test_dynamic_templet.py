from generation.templet import stringfunction

def test_dynamic_templet():
    #pylint: disable=unused-argument
    def func(arg1, arg2):
        pass

    func.__doc__ = "a=$arg1 b=$arg2"

    res = stringfunction(func)

    assert res(1, 2) == "a=1 b=2"
