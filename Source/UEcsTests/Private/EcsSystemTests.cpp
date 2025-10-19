// Boilerplate CQTest suite for UEcs::EcsSystem
// NOTE: This is only a scaffold. Add real test cases later.

#include "CoreMinimal.h"
#include "CQTest.h" 

#include "EcsSystem.h"

// A simple fixture to share setup/teardown for EcsSystem-related tests
struct FEcsSystemFixture
{
    void BeforeEach()
    {
        // Initialize common state for each test here
    }

    void AfterEach()
    {
        // Cleanup per-test state here
    }
};

TEST_CLASS(UEcs_EcsSystem, "UEcs.System")
{
    //static variables setup in BEFORE_ALL and torn down in AFTER_ALL

    //Member variables shared between tests

    BEFORE_ALL()
    {
    }

    BEFORE_EACH()
    {
    }

    AFTER_EACH()
    {
    }

    AFTER_ALL()
    {
    }

    TEST_METHOD(When_Given_Expect)
    {
        ASSERT_THAT(IsTrue(true));
    }
};
