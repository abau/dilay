#include <utility>
#include <glm/glm.hpp>
#include <QSpinBox>
#include "tool/util/integer.hpp"
#include "tool.hpp"
#include "view/tool-options.hpp"
#include "view/util.hpp"
#include "config.hpp"

struct ToolUtilInteger::Impl {
  ToolUtilInteger*   self;
  const std::string& config;
  const int          min;
  const int          max;
  int                value;
  QSpinBox&          valueEdit;

  Impl (ToolUtilInteger* s, const std::string& c, int mi, int _default, int ma) 
    : self      (s) 
    , config    (c)
    , min       (mi)
    , max       (ma)
    , value     (glm::clamp (Config::get <int> (c, _default), min, max))
    , valueEdit (ViewUtil::spinBox (min, this->value, max))
  {
    ViewUtil::connect (this->valueEdit, [this] (int v) { this->setValue (v,false); });
  }

  ~Impl () {
    Config::cache <int> (this->config, this->value);
  }

  void setValue (int newValue, bool updateEdit = true) {
    if (this->value != newValue && newValue <= this->max && newValue >= this->min) {
      this->value = newValue;

      if (updateEdit) {
        this->valueEdit.setValue (newValue);
      }
    }
  }

  void increase () { this->setValue (this->value + 1); }
  void decrease () { this->setValue (this->value - 1); }
};

DELEGATE_BIG3_BASE ( ToolUtilInteger,(Tool& t, const std::string& c, int mi, int d, int ma)
                   , (this,c,mi,d,ma),ToolUtil,(t))
GETTER_CONST (int , ToolUtilInteger, value)
DELEGATE     (void, ToolUtilInteger, increase)
DELEGATE     (void, ToolUtilInteger, decrease)
