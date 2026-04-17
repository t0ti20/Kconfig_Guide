#include <cstdio>
#include <cstring>
#include <ctime>
#include <cctype>
#include <thread>
#include <chrono>
#include <fstream>

// autoconf.h is generated at build time from the .config file.
// It turns every Kconfig symbol into a C preprocessor macro.
#include "autoconf.h"

// =============================================================================
// ANSI Color Codes
// =============================================================================

#ifdef CONFIG_USE_COLORS
    #ifdef CONFIG_COLOR_RED
        #define ANSI_COLOR "\033[31m"
    #elif defined(CONFIG_COLOR_GREEN)
        #define ANSI_COLOR "\033[32m"
    #elif defined(CONFIG_COLOR_YELLOW)
        #define ANSI_COLOR "\033[33m"
    #elif defined(CONFIG_COLOR_BLUE)
        #define ANSI_COLOR "\033[34m"
    #elif defined(CONFIG_COLOR_MAGENTA)
        #define ANSI_COLOR "\033[35m"
    #elif defined(CONFIG_COLOR_CYAN)
        #define ANSI_COLOR "\033[36m"
    #else
        #define ANSI_COLOR ""
    #endif

    #ifdef CONFIG_BOLD_TEXT
        #define ANSI_BOLD "\033[1m"
    #else
        #define ANSI_BOLD ""
    #endif

    #define ANSI_RESET "\033[0m"
#else
    #define ANSI_COLOR ""
    #define ANSI_BOLD ""
    #define ANSI_RESET ""
#endif

// =============================================================================
// Helper Functions
// =============================================================================

void log_to_file(const char* message) {
#ifdef CONFIG_ENABLE_LOGGING
    std::ofstream logfile(CONFIG_LOG_FILENAME, std::ios::app);
    if (logfile.is_open()) {
        #ifdef CONFIG_TIMESTAMP_FORMAT
            time_t now = time(nullptr);
            char timestamp[32];
            strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
            logfile << "[" << timestamp << "] ";
        #endif
        logfile << message << std::endl;
    }
#endif
}

void print_message(const char* msg) {
    char buffer[CONFIG_BUFFER_SIZE];
    strncpy(buffer, msg, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    #ifdef CONFIG_UPPERCASE_MODE
        for (size_t i = 0; buffer[i]; i++) {
            buffer[i] = toupper(buffer[i]);
        }
    #endif

    #ifdef CONFIG_ASSERT_ENABLED
        size_t len = strlen(buffer);
        if (len > CONFIG_MAX_MESSAGE_LENGTH) {
            fprintf(stderr, "ERROR: Message length (%zu) exceeds maximum (%d)\n",
                    len, CONFIG_MAX_MESSAGE_LENGTH);
            return;
        }
    #endif

    #ifdef CONFIG_ENABLE_OUTPUT
        printf("%s%s%s%s\n", ANSI_BOLD, ANSI_COLOR, buffer, ANSI_RESET);
        log_to_file(buffer);
    #endif
}

void print_border_top() {
    #ifdef CONFIG_BORDER_STYLE
        #ifdef CONFIG_BORDER_UNICODE
            printf("┌────────────────────────────────────────────────────────┐\n");
        #else
            printf("+--------------------------------------------------------+\n");
        #endif
    #endif
}

void print_border_bottom() {
    #ifdef CONFIG_BORDER_STYLE
        #ifdef CONFIG_BORDER_UNICODE
            printf("└────────────────────────────────────────────────────────┘\n");
        #else
            printf("+--------------------------------------------------------+\n");
        #endif
    #endif
}

void print_bordered_message(const char* msg) {
    #ifdef CONFIG_BORDER_STYLE
        #ifdef CONFIG_BORDER_UNICODE
            printf("│ %-54s │\n", msg);
        #else
            printf("| %-54s |\n", msg);
        #endif
    #else
        printf("%s\n", msg);
    #endif
}

void show_config_dump() {
    #ifdef CONFIG_SHOW_CONFIG_VALUES
        printf("\n=== Configuration Dump ===\n");
        printf("ENABLE_OUTPUT         = %s\n",
               #ifdef CONFIG_ENABLE_OUTPUT
                   "true"
               #else
                   "false"
               #endif
        );
        #ifdef CONFIG_ENABLE_OUTPUT
            printf("GREETING_MESSAGE      = \"%s\"\n", CONFIG_GREETING_MESSAGE);
            printf("SECONDARY_MESSAGE     = \"%s\"\n", CONFIG_SECONDARY_MESSAGE);
            printf("USE_COLORS            = %s\n",
                   #ifdef CONFIG_USE_COLORS
                       "true"
                   #else
                       "false"
                   #endif
            );
            #ifdef CONFIG_USE_COLORS
                printf("BOLD_TEXT             = %s\n",
                       #ifdef CONFIG_BOLD_TEXT
                           "true"
                       #else
                           "false"
                       #endif
                );
            #endif
        #endif
        printf("REPEAT_COUNT          = %d\n", CONFIG_REPEAT_COUNT);
        printf("DELAY_MS              = %d\n", CONFIG_DELAY_MS);
        printf("BUFFER_SIZE           = 0x%x (%d bytes)\n", CONFIG_BUFFER_SIZE, CONFIG_BUFFER_SIZE);
        #ifdef CONFIG_ENABLE_LOGGING
            printf("ENABLE_LOGGING        = true\n");
            printf("LOG_FILENAME          = \"%s\"\n", CONFIG_LOG_FILENAME);
        #endif
        printf("DEBUG_MODE            = %s\n",
               #ifdef CONFIG_DEBUG_MODE
                   "true"
               #else
                   "false"
               #endif
        );
        printf("==========================\n\n");
    #endif
}

void show_verbose_info() {
    #ifdef CONFIG_VERBOSE_MODE
        printf("\n");
        printf("Application Version:  %s\n", CONFIG_APP_VERSION);
        printf("Author:               %s\n", CONFIG_APP_AUTHOR);

        #ifdef CONFIG_SHOW_COPYRIGHT
            printf("Copyright:            (C) %s %s\n",
                   CONFIG_COPYRIGHT_YEAR, CONFIG_APP_AUTHOR);
        #endif

        time_t now = time(nullptr);
        printf("Current Time:         %s", ctime(&now));

        #ifdef CONFIG_MEMORY_STATS
            printf("\nMemory Statistics:\n");
            printf("  Buffer Size:        %d bytes (0x%x)\n",
                   CONFIG_BUFFER_SIZE, CONFIG_BUFFER_SIZE);
            #ifdef CONFIG_ASSERT_ENABLED
                printf("  Max Message Length: %d characters\n", CONFIG_MAX_MESSAGE_LENGTH);
            #endif
        #endif

        printf("\n");
    #endif
}

// =============================================================================
// Main Application
// =============================================================================

int main()
{
    #ifdef CONFIG_DEBUG_MODE
        printf("[DEBUG] Application starting...\n");
        show_config_dump();
    #endif

    #ifdef CONFIG_VERBOSE_MODE
        show_verbose_info();
    #endif

    #ifdef CONFIG_ENABLE_OUTPUT
        print_border_top();

        // Main message
        for (int i = 0; i < CONFIG_REPEAT_COUNT; i++) {
            #ifdef CONFIG_BORDER_STYLE
                print_bordered_message(CONFIG_GREETING_MESSAGE);

                // Secondary message
                if (strlen(CONFIG_SECONDARY_MESSAGE) > 0) {
                    print_bordered_message(CONFIG_SECONDARY_MESSAGE);
                }
            #else
                print_message(CONFIG_GREETING_MESSAGE);

                // Secondary message
                if (strlen(CONFIG_SECONDARY_MESSAGE) > 0) {
                    print_message(CONFIG_SECONDARY_MESSAGE);
                }
            #endif

            // Delay between repetitions (except after last)
            #ifdef CONFIG_DELAY_MS
                if (i < CONFIG_REPEAT_COUNT - 1 && CONFIG_DELAY_MS > 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(CONFIG_DELAY_MS));
                }
            #endif
        }

        print_border_bottom();
    #endif

    #ifdef CONFIG_DEBUG_MODE
        printf("[DEBUG] Application finished successfully.\n");
    #endif

    return 0;
}
