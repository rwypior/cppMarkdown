#include "extensions.h"

#include "ext/tableelement.h"

namespace Markdown
{
    // Standard extension

    void StandardExtension::extend(ParserCollection& parsers)
    {

    }

    // Extension manager

    ExtensionsManager& ExtensionsManager::getInstance()
    {
        static ExtensionsManager manager;
        return manager;
    }

    void ExtensionsManager::registerExtension(std::unique_ptr<Extension>&& extension)
    {
        this->extensions.push_back(std::move(extension));
    }

    void ExtensionsManager::extend(ParserCollection& parsers)
    {
        for (auto& extension : this->extensions)
        {
            extension->extend(parsers);
        }
    }

    // Utils

    void registerStandardExtensions()
    {
        ExtensionsManager& manager = ExtensionsManager::getInstance();
        manager.registerExtension(std::make_unique<StandardExtension>());
    }
}
