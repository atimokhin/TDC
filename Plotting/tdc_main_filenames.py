class tdc_main_Filenames:
    """
    class with methods for retriving full filename and setting results directory
        RESULTS     -- computation data
    """

    __default_ResultsDir    ='../RESULTS/'

    def __init__(self):
        import argparse
        parser = argparse.ArgumentParser()
        parser.add_argument('--results_dir', nargs='?', type=str, help='RESULTS directory')
        parser.add_argument('--id', nargs='?', type=str, help='calculation ID')
        args = parser.parse_known_args()

        if args[0].id:
            self.__id_dir=args[0].id
            if args[0].results_dir:
                self.__results_dir=args[0].results_dir
            else:
                self.__results_dir=self.__default_ResultsDir
        else:
            self.__id_dir=None
            self.__results_dir=None


    def set_results_dir(self, name=__default_ResultsDir):
        if self.__id_dir:
            self.__results_dir=name
        else:
            print 'no ID is set -> setting results_dis has no sense'

    def get_results_dir(self):
        return self.__results_dir

    def get_full_filename(self,filename):
        if self.__id_dir:
            return self.__results_dir + '/' + self.__id_dir + '/' + filename
        else:
            return filename
        

