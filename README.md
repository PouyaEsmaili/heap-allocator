# heap-allocator

<div dir="rtl" align="justify">
 در این پروژه یک تخصیصگر حافظه Heap مشابه با malloc پیاده‌سازی کرده‌ایم.
  
  قسمت اصلی این پروژه در فایل mem.c نوشته شده است و در فایل main.c تست پروژه را نوشته‌ایم.
  
  <h2> شرح ساختار فایل mem.c </h2>
  
  در این فایل 4 تابع اصلی و چند تابع کمکی پیاده‌سازی شده است. چهار تابع اصلی به شرح زیر است:
  
  * mem_alloc(int size)
  * mem_free(void* ptr)
  * mem_GetSize(void* ptr)
  * mem_IsValid(void *ptr)
  
  برای پیاده‌سازی این توابع به چند تابع کمکی هم نیاز داشتیم که در فایل mem.c قابل مشاهده هستند.
  
  در ابتدا یک linked list نیاز داریم که برای اینکار یک struct تعریف میکنیم که با نام block_header_t در فایل قابل مشاهده است.
  این struct شامل دو مقدار size و actual size است که size مقداری است که کاربر استفاده میکند و actual size مقدار فضایی است که کاربر در اختیار دارد و ممکن است
  بخشی از آن استفاده نشود در واقع این actual size نشان دهنده internal fragmentation است.
  همچنین actual size شامل اندازه header هم میشود اما size، تنها اندازه حافظه است.
  
  از struct ای که در بالا توضیح دادیم دو instance یعنی یک free_list و یک used_list میسازیم.
  لینک لیست free_list در ابتدا کل حافظه‌ای است که داریم و used_list در ابتدا خالی است و در ادامه اجرای برنامه وقتی کاربر یک آبجکتی درخواست میکند، به اندازه آن
  حافظه به used_list اضافه و از free_list کم میشود.
  
  <b> تابع mem_init </b>

  در تابع mem_init، مقداردهی‌های اولیه را انجام میدهد و بخشی از حافظه را میگیرد.
این تابع مقدار sizeOfRegion به صورت ورودی میگیرد و آن را به بالا گرد میکند تا این متغیر مضرب page_size شود و به اندازه آن حافظه گرفته شده است.  
  <b>تابع mem_alloc </b>
  
  برای این پیاده‌سازی این تابع به چند تابع کمکی نیاز داریم که در ادامه به توضیح آنها میپردازیم. تابع find_free_block، برای پیدا کردن best_fit کمک میکند.
  
  تابع split_block، مقدار best_fit که از تابع find_free_block پیدا کردیم را بررسی میکند.
  در صورتیکه بیشتر از 8 بایت از best_fit خالی بماند، این تابع آن block را به دو block تقسیم میکند و یکی از block ها را تخصیص میدهیم و block دیگر 
  را در free_block نگه میداریم. اما اگر کمتر از 8 بایت فضای خالی باقی مانده باشد block را تقسیم نمیکند و مقدار size را ست میکند.
  
  تابع assign_block وظیفه دارد تا block را از free_list خارج کند و آن را اول used_list قرار دهد.
  
  هر حافظه‌ای که کاربر از ما میگیرد، Header آن را ابتدای used_list قرار میدهیم. اما در free_list بلاک‌ها ترتیب دارند یعنی اینکه اگر block2 برابر باشد با next-> block1
  میتوانیم نتیجه بگیریم که آدرس block2 هم بزرگتر از آدرس block1 است.
  
  تابع mem_alloc از توابعی که در بالا توضیح دادیم استفاده میکند و یک block خالی پیدا میکند. در صورتیکه یک بلاک پیدا کند، آن را split کرده و assign میکنیم
  و آدرس شروع حافظه را برمیگردانیم. اگر هیچ block خالی ای پیدا نکرد، null برمیگرداند.
  
  <b>تابع find_block</b>
  
  این تابع با گرفتن pointer بررسی میکند که آیا این pointer در بین حافظه هیچ یک از block ها وجود دارد یا خیر. نحوه انجام آن هم بدین شکل است که بر روی used_list، گذر
  میکنیم و یکی یکی block های آن را بررسی میکنیم و اگر pointer در یکی از این block ها بود، آن را retunrn میکند و در غیر اینصورت، null برمیگرداند.
  
  <b>تابع put_in_free_list</b>
  
  هدف از پیاده‌سازی این تابع آن است که یک بلاک را در جای مناسب خود در free_list پیدا کنیم و آن را به این لیست اضافه کنیم. به دلیل اینکه در free_list ترتیب برای
  ما مهم است، این تابع را پیاده‌سازی کردیم.
  
  <b>تابع merge_blocks</b>
  
  این تابع وظیفه دارد تا روی free_list گذر کند و اگر دو block ای که پشت سر هم باشند، خالی باشند برای جلوگیری از رخ دادن fragmentation
  این دو block را با هم merge میکنیم و یک block بزرگتر تولید میکنیم.
  
  <b>تابع Mem_free </b>
  
  این تابع از سه تابعی که در بالا توضیح دادیم یعنی find_block، put_in_free_list و merge_blocks استفاده میکند. این تابع ابتدا یک block خالی پیدا میکند و آن
  را در free_list قرار میدهد و از used_list حذف میکنیم. در صورتیکه نیاز باشد هم merge_blocks، بلاک های پشت سرهم را ادغام میکند.
  
  <b>تابع Mem_IsValid</b>
  
  این تابع با گرفتن یک pointer به ما اعلام میکند که آیا بلاکی وجود دارد و اگر وجود داشت عدد 1 را return میکند و در غیر اینصورت عدد 0 را return میکند.
  
  <b>تابع Mem_GetSize </b>
  
  این تابع هم مانند تابع Mem_IsValid یک pointer ورودی میگیرد و اگر block را پیدا کند، اندازه آن را return میکند، در غیراینصورت -1 برمیگرداند.
  

  <h2> شرح فایل تست main.c </h2>

در این فایل تست پروژه نوشته شده است و طبق مورد خواسته شده در داک پروژه این فایل همه حالت های مختلف را تست میکند.
</div>
  
