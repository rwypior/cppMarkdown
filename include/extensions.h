#ifndef _h_cppmarkdownextensions
#define _h_cppmarkdownextensions

#include "cppmarkdowncommon.h"

#include <vector>
#include <memory>

namespace Markdown
{
    class Extension
    {
    public:
        virtual ~Extension() = default;
        virtual void extend(ParserCollection& parsers) = 0;
    };

    class StandardExtension : public Extension
    {
    public:
        virtual void extend(ParserCollection& parsers) override;
    };

    class ExtensionsManager
    {
    public:
        static ExtensionsManager& getInstance();

        void registerExtension(std::unique_ptr<Extension>&& extension);

        void extend(ParserCollection& parsers);

    protected:
        ExtensionsManager() = default;

        std::vector<std::unique_ptr<Extension>> extensions;
    };

    void registerStandardExtensions();
}

#endif
