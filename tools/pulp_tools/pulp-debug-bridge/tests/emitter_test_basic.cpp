#include "events/emitter.hpp"
#include <assert.h>

SMART_EMITTER(Test, test)

class TestClass : public TestEmitter<int> {

};

class TestRef {
public:
    TestRef(EmitterRef &ref) : m_ref(ref) {}
    EmitterRef &m_ref;
};

int main( int argc, const char* argv[] )
{
    TestClass test;
    TestRef * ref;
    test.once_test([&](int i){
        printf("# got1-1 %d\n", i);
        assert(i==1);
        test.clear_test(ref->m_ref);
    });
    ref = new TestRef(test.on_test([](int i){
        printf("!!!shouldn't fire %d\n", i);
        exit(1);
    }));
    test.once_test([&](int i){
        printf("# got1-2 %d\n", i);
        assert(i==1);
    });
    test.emit_test(1);
    delete ref;
    test.on_test([&](int i){
        printf("# got2 %d\n", i);
        test.clearall_test();
    });
    ref = new TestRef(test.on_test([](int i){
        printf("!!!shouldn't fire %d\n", i);
        exit(1);
    }));
    test.emit_test(2);
    delete ref;
    printf("Test Passed\n");
}