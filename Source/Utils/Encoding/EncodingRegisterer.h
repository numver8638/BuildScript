#ifndef BUILDSCRIPT_UTILS_ENCODING_ENCODINGREGISTERER_H
#define BUILDSCRIPT_UTILS_ENCODING_ENCODINGREGISTERER_H

#ifndef BUILDSCRIPT_UTILS_ENCODING_H
    #error "Cannot include EncodingRegisterer.h solely. Did you forget include Encoding.h?"
#endif

template <typename EncodingT>
class EncodingRegisterer {
private:
    EncodingT m_encoding;

public:
    explicit EncodingRegisterer() noexcept {
        BuildScript::Encoding::Register(&m_encoding);
    }
};

#define REGISTER_ENCODING(encoding)                                     \
    namespace {                                                         \
        EncodingRegisterer<encoding> __registered_##encoding;  \
    }

#endif // BUILDSCRIPT_UTILS_ENCODING_ENCODINGREGISTERER_H