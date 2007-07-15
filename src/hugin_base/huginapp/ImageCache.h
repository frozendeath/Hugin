// -*- c-basic-offset: 4 -*-
/** @file ImageCache.h
 *
 *  @author Pablo d'Angelo <pablo.dangelo@web.de>
 *
 *  $Id: ImageCache.h 1988 2007-05-08 22:55:04Z dangelo $
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _IMAGECACHE_H
#define _IMAGECACHE_H

#include <map>
#include <boost/shared_ptr.hpp>
#include <vigra/stdimage.hxx>
#include <vigra/imageinfo.hxx>
#include <hugin_utils/utils.h>
#include <appbase/ProgressDisplayOld.h>

#define HUGIN_IMGCACHE_MAPPING_INTEGER        0l
#define HUGIN_IMGCACHE_MAPPING_FLOAT          1l


namespace HuginBase {
    
/** This is a cache for all the images we use.
 *
 *  is a singleton for easy access from everywhere.
 *  The cache is used as an image source, that needs
 *  to know how to reproduce the requested images, in case
 *  that they have been deleted.
 *
 *  @todo: implement a strategy for smart deletion of images
 *  @todo: add more advanced key, that stores access statistics
 *         and so on.
 */
class ImageCache
{
        
    public:
        /// use reference counted pointers
        typedef boost::shared_ptr<vigra::BRGBImage> ImageCacheRGB8Ptr;
        typedef boost::shared_ptr<vigra::FRGBImage> ImageCacheRGBFloatPtr;
        typedef boost::shared_ptr<vigra::BImage> ImageCache8Ptr;
        
        /** information about an image inside the cache */
        struct Entry
        {
            ImageCacheRGB8Ptr image8;
            ImageCacheRGBFloatPtr imageFloat;
            ImageCache8Ptr mask;

            std::string origType;
            int lastAccess;
                
            public:
                ///
                Entry()
                : image8(ImageCacheRGB8Ptr(new vigra::BRGBImage)),
                  imageFloat(ImageCacheRGBFloatPtr(new vigra::FRGBImage)),
                  mask(ImageCache8Ptr(new vigra::BImage))
                {};

                ///
                Entry(ImageCacheRGB8Ptr & img,
                      ImageCacheRGBFloatPtr & imgFloat,
                      ImageCache8Ptr & imgMask,
                      const std::string & typ)
                : image8(img), imageFloat(imgFloat), mask(imgMask), origType(typ), lastAccess(0)
                {};
        };

        /** a shared pointer to the entry */
        typedef boost::shared_ptr<Entry> EntryPtr;

        
    private:
        // ctor. private, nobody execpt us can create an instance.
        ImageCache()
            : m_progress(NULL), m_accessCounter(0)
        {};
        
    public:
        /** dtor.
         */
        virtual ~ImageCache()
        {
                images.clear();
                //instance = NULL;
        }

        /** get the global ImageCache object */
        static ImageCache & getInstance();
        
    private:
        static ImageCache* instance;

        
    public:
        /** get a image.
         *
         *  it will be loaded if its not already in the cache
         *
         *  Hold the EntryPtr as long as the image data is needed!
         */
        EntryPtr getImage(const std::string & filename, bool forceInteger=false);

        /** get an small image.
         *
         *  This image is 512x512 pixel maximum and can be used for icons
         *  and different previews. It is directly derived from the original.
         */
        EntryPtr getSmallImage(const std::string & filename,
                               bool forceInteger=false);


        /** remove a specific image (and dependant images)
         * from the cache 
         */
        void removeImage(const std::string & filename);

        /** release all images in the cache.
         *
         *  useful on project load, or maybe before stitching really
         *  big pictures
         */
        void flush();

        /** a soft version of flush.
         *
         *  Releases some images if they go over a certain threshold
         */
        void softFlush();

    private:
        template <class T1>
        class GetRange
        {
            public:
                static T1 min();
                static T1 max();
        };
        
        void convertTo8Bit(vigra::FRGBImage& src,
                           std::string origType,
                           vigra::BRGBImage & dest);
        
        template <class SrcPixelType,
                  class DestIterator, class DestAccessor>
        void importAndConvertImage(const vigra::ImageImportInfo& info,
                                   vigra::pair<DestIterator, DestAccessor> dest,
                                   const std::string& type);
        
    //    template <class SrcPixelType,
    //              class DestIterator, class DestAccessor>
    //    void importAndConvertGrayImage(const ImageImportInfo& info,
    //                                   vigra::pair<DestIterator, DestAccessor> dest,
    //                                   wxString type);
        
    //    template <class SrcPixelType,
    //              class DestIterator, class DestAccessor>
    //    void importAndConvertGrayAlphaImage(const ImageImportInfo & info,
    //                                        vigra::pair<DestIterator, DestAccessor> dest,
    //                                        wxString type);
        
        template <class SrcPixelType,
                  class DestIterator, class DestAccessor,
                  class MaskIterator, class MaskAccessor>
            void importAndConvertAlphaImage(const vigra::ImageImportInfo & info,
                                        vigra::pair<DestIterator, DestAccessor> dest,
                                        vigra::pair<MaskIterator, MaskAccessor> mask,
                                        const std::string & type);
        
        
    public:
        ///
        void setProgressDisplay(AppBase::MultiProgressDisplay* disp)
            { m_progress = disp; }
        
        ///
        void clearProgressDisplay(AppBase::MultiProgressDisplay* disp)
            { m_progress = NULL; }
        
        
    private:
        std::map<std::string, EntryPtr> images;

        // our progress display
        AppBase::MultiProgressDisplay* m_progress;

        int m_accessCounter;
        
        
    public:
        /** get a pyramid image.
         *
         *  A image pyramid is a image in multiple resolutions.
         *  Usually it is used to accelerate image processing, by using
         *  lower resolutions first. they are properly low pass filtered,
         *  so no undersampling occurs (it would if one just takes
         *  every 2^level pixel instead).
         *
         *  @param filename of source image
         *  @param level of pyramid. height and width are calculated as
         *         follows: height/(level^2), width/(level^1)
         *
         */
    //    const vigra::BImage & getPyramidImage(const std::string& filename,
    //                                          int level);

    private:
        // key for your pyramid map.
        struct PyramidKey
        {
            std::string filename;
            int level;
            
            public:
                PyramidKey(const std::string& str, int lv)
                  : filename(str), level(lv)
                {};
            
                std::string toString()
                    { return filename + hugin_utils::lexical_cast<std::string>(level); }
        };
        
        std::map<std::string, vigra::BImage *> pyrImages;
};


} //namespace
#endif // _IMAGECACHE_H
