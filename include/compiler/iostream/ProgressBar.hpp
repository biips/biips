/*! \file ProgressBar.hpp
 * COPY: Adapted from boost::progress_display class in boost/progress.hpp
 */

#ifndef PROGRESSBAR_HPP_
#define PROGRESSBAR_HPP_

#include <boost/progress.hpp>

namespace Biips
{

  class ProgressBar
  {
  public:
    ProgressBar(unsigned long expected_count,
                std::ostream & os = std::cout,
                const std::string & s1 = "\n", //leading string
                char symbol = '*',
                const std::string & iter_name = "iterations",
                bool display_time = true);

    //! \brief Display appropriate scale
    /*! \post count()==0, expected_count()==expected_count */
    void restart(unsigned long expected_count,
                 char symbol = '*',
                 const std::string & iter_name = "iterations",
                 bool display_bar = true);
    //! \brief Display appropriate progress tic if needed.
    //! \post count()== original count() + increment
    //! \return count().
    unsigned long operator+=(unsigned long increment);
    unsigned long operator++();
    unsigned long count() const;
    unsigned long expected_count() const;

  protected:
    std::ostream & os_; // may not be present in all imps
    const std::string s1_; // string is more general, safer than
    // const char *, and efficiency or size are

    unsigned long count_, expected_count_, next_tic_count_;
    unsigned int tic_;
    std::string iter_name_;
    char symbol_;
    boost::timer timer_;
    bool display_time_;

    void display_tic();
  };

}

#endif /* PROGRESSBAR_HPP_ */
