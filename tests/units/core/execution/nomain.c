/**
 * @file        nomain.c
 * @version     0.1.0
 * @brief       Core module unit tests for library-defined main.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 */

#include <assert.h>

#include "sccroll.h"

// clang-format off

/******************************************************************************
 * Preparation
 ******************************************************************************/
// clang-format on

// Simple test printing on stdout.
SCCROLL_TEST(test_print, .std[STDOUT_FILENO].content.blob="Test executed.") { puts("Test executed."); }

// clang-format off

/******************************************************************************
 * Execution
 ******************************************************************************/
// clang-format on

/*
 * Nothing here as the test should be automatically executed.
 * TODO: merge with the core/execution/main.c test, using macros
 * definitions to distinguish the cases.
 */
