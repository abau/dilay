#include <utility>
#include <QSpinBox>
#include "tool/util/integer.hpp"
#include "tool.hpp"
#include "view/tool-options.hpp"
#include "view/util.hpp"
#include "config.hpp"

struct ToolUtilInteger::Impl {
  ToolUtilInteger*   self;
  const std::string& config;
  int                value;
  QSpinBox&          valueEdit;

  Impl (ToolUtilInteger* s, const std::string& c, int min, int _default, int max) 
    : self      (s) 
    , config    (c)
    , value     (Config::get <int> (c, _default))
    , valueEdit (ViewUtil::spinBox (min, this->value, max))
  {
    ViewUtil::connect (this->valueEdit, [this] (int v) { this->value = v; });
  }

  ~Impl () {
    Config::cache <int> (this->config, this->value);
  }
};

DELEGATE_BIG3_BASE ( ToolUtilInteger,(Tool& t, const std::string& c, int mi, int d, int ma)
                   , (this,c,mi,d,ma),ToolUtil,(t))
GETTER_CONST (int, ToolUtilInteger, value)
