//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.

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
