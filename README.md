# News Crunch 📰

**News Crunch** is a multithreaded desktop news aggregator built in C++ using the SplashKit framework. It connects directly to the NewsAPI to stream live global headlines and integrates local AI language models (Qwen-3.1-7B) to summarize news feeds and individual articles on demand.

### 🚀 Key Features
*   **Asynchronous Content Fetching:** Performs HTTP GET queries on a background thread to prevent UI freezing and ensure a smooth 60 FPS user experience.
*   **AI Summarization:** Integrates `QWEN3_1_7B_INSTRUCT` to generate a one-sentence overview of the feed and 3-to-4 sentence article summaries in a dynamic popup panel.
*   **Parallel Image Downloads:** Spawns concurrent worker threads to pull article thumbnails in parallel and uses thread joins to synchronize data dynamically.
*   **User Profiles & History:** Saves user credentials, reading stats, favorite sources, and search queries locally inside a serialized JSON file database.
*   **Robust Rendering:** Implements responsive card grid math, viewport frustum culling (skipping rendering for off-screen cards), and clipping boundaries for a clean layout.
*   **Null-Safe JSON Parser:** Implements a custom pre-processor that sanitizes incoming API payloads by transforming null JSON fields into empty strings to avoid parsing errors.

### 🛠️ Tech Stack
*   **Language:** C++
*   **Engine & GUI:** SplashKit (Windowing, Graphics, GUI panels)
*   **APIs:** NewsAPI (Real-time articles)
*   **AI Engine:** SplashKit LLM Engine (Qwen model)
*   **Storage:** Local JSON files
